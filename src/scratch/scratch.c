#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "../common/bytecode.h"
//#include "../common/opcodes.h"
//#include "../common/error.h"
#include "../common/debug.h"
#include "../common/version.h"

enum mode_e {
    NONE,
    MODE_STRINGS,
    MODE_VARIABLES,
    MODE_FUNCTIONS,
    DONE,
};

// Create a lookup table so that we can see opcode names during debugging.
static char *opcode_name_table[] = {
// Abuse of stringification
#define OPCODE(name, _) "" #name "",
#include "../common/opcodes.h"
#undef OPCODE
};

// TODO 200 lines to parse a seemingly simple bytecode format seems excessive..........
int main(int argc, char **argv) {
    printf("git short hash: %s\n", GIT_SHORT_HASH);
    printf("git long hash: %s\n", GIT_LONG_HASH);
    printf("build date: %s\n", DATE);
    if(argc < 2) {
        fprintf(stderr, RED "no file passed\n" RESET);
        exit(1);
    }

    DEBUG("file: %s\n", argv[1])
    FILE *file = fopen(argv[1], "rb");
    // Buffer to allocate
    uint8_t *buffer;
    // Jump to the end of the file
    fseek(file, 0, SEEK_END);
    // Get current offset into file (in bytes)
    long size = ftell(file);
    DEBUG("size: %ld\n", size)
    // Jump back to the beginning of the file
    rewind(file);

    // Allocate a buffer to fit file contents + '\0'
    buffer = calloc((size + 1), sizeof(uint8_t));
    fread(buffer, size, 1, file);
    fclose(file);
    // Note: When we do this memcpy, the endianness of the machine causes this
    // value to be reversed, so we instead stitch it together with bitshifts.
    // 
    // See: http://stackoverflow.com/questions/17435115/memcpy-from-byte-to-unsigned-int-is-reversing-byte-order
    // See: https://stackoverflow.com/questions/16008879/how-do-you-write-portably-reverse-network-byte-order
    // See: http://stackoverflow.com/questions/5874311/reversing-endianness-with-memcpy
    // See: http://programmers.stackexchange.com/questions/215535/regarding-little-endian-and-big-endian-conversion
    // See: http://stackoverflow.com/questions/9144800/c-reverse-bits-in-unsigned-integer
    // 
    // uint64_t magic_number;
    // memcpy(&magic_number, buffer, sizeof(magic_number));
    
    // Stitch together magic constant from first 8 bytes
    const uint64_t magic_number = ((uint64_t) buffer[0] << 56) | 
                                  ((uint64_t) buffer[1] << 48) |
                                  ((uint64_t) buffer[2] << 40) | 
                                  ((uint64_t) buffer[3] << 32) |
                                  ((uint64_t) buffer[4] << 24) | 
                                  ((uint64_t) buffer[5] << 16) | 
                                  ((uint64_t) buffer[6] << 8)  | 
                                  ((uint64_t) buffer[7] << 0);

    if(magic_number != MAGIC_NUMBER) {
        fprintf(stderr, RED "not valid bytecode: %s\n" RESET, argv[1]);
        fprintf(stderr, RED "magic:    0x%lx\n" RESET, magic_number);
        fprintf(stderr, RED "expected: 0x%lx\n" RESET, MAGIC_NUMBER);
        goto end;
    }

    enum mode_e mode = NONE;
    // Read to the end of the buffer
    int string_counter = 0;
    int variable_counter = 0;
    int function_counter = 0;
    bool valid_string_header = false;
    bool valid_variable_header = false;
    bool valid_function_header = false;
    for(int i = 8; i < size && mode != DONE; i++) {
        if(mode == NONE) {
            char *section_header = malloc((strlen(SECTION_HEADER_STRINGS) + 1) * sizeof(char));
            for(; i <= 15; i++) {
                section_header[i - 8] = buffer[i];
            }
            section_header[8] = '\0';
            DEBUG("header: '%s'\n", section_header)
            if(strncmp(section_header, SECTION_HEADER_STRINGS, strlen(SECTION_HEADER_STRINGS)) != 0) {
                fprintf(stderr, RED "'%s' isn't a valid string section header! (expected '%s')\n" RESET, 
                        section_header, SECTION_HEADER_STRINGS);
                free(section_header);
                goto end;
            } else {
                valid_string_header = true;
                mode = MODE_STRINGS;
            }
            free(section_header);
        }
        if(mode == MODE_STRINGS) {
            uint32_t location = ((uint32_t) buffer[i    ] << 24) |
                                ((uint32_t) buffer[i + 1] << 16) |
                                ((uint32_t) buffer[i + 2] << 8)  |
                                ((uint32_t) buffer[i + 3]);
            i += 4; // Move past int
            uint32_t length = ((uint32_t) buffer[i    ] << 24) |
                              ((uint32_t) buffer[i + 1] << 16) |
                              ((uint32_t) buffer[i + 2] << 8)  |
                              ((uint32_t) buffer[i + 3]);
            i += 3; // Move past int, then back 1 because i++ at the end of the loop
            DEBUG("location: 0x%x\n", location)
            DEBUG("length:   0x%x\n", length)
            //                . v a r                 i a b l
            if(location == 0x2E766172 && length == 0x6961626C) {
                // Need to undo the changes to i from length/location
                DEBUG(".strings end\n")
                mode = MODE_VARIABLES;
                i -= 8;
                continue;
            }
            DEBUG("found string: '%.*s'\n", length, buffer + location);
            printf("string %d: %.*s\n", string_counter++, length, buffer + location);
        }
        if(mode == MODE_VARIABLES) {
            if(!valid_variable_header) {
                unsigned int len = strlen(SECTION_HEADER_VARIABLES);
                char *section_header = malloc((len + 1) * sizeof(char));
                for(unsigned int j = 0; j <= len; j++) {
                    section_header[j] = buffer[i + j];
                }
                i += len;
                section_header[len] = '\0';
                DEBUG("header: %s\n", section_header)
                if(strncmp(section_header, SECTION_HEADER_VARIABLES, strlen(SECTION_HEADER_VARIABLES)) != 0) {
                    fprintf(stderr, RED "'%s' isn't a valid string section header! (expected '%s')\n" RESET, 
                            section_header, SECTION_HEADER_VARIABLES);
                    free(section_header);
                    goto end;
                }
                valid_variable_header = true;
                free(section_header);
            }
            uint32_t name = ((uint32_t) buffer[i    ] << 24) |
                            ((uint32_t) buffer[i + 1] << 16) |
                            ((uint32_t) buffer[i + 2] << 8)  |
                            ((uint32_t) buffer[i + 3]);
            i += 4; // Move past int
            uint32_t type = ((uint32_t) buffer[i    ] << 24) |
                            ((uint32_t) buffer[i + 1] << 16) |
                            ((uint32_t) buffer[i + 2] << 8)  |
                            ((uint32_t) buffer[i + 3]);
            i += 3; // Move past int, then back 1 because i++ at the end of the loop
            
            //            . f u n               c t i o
            if(name == 0x2E66756E && type == 0x6374696F) {
                // Need to undo the changes to i from length/location
                DEBUG(".variables end\n")
                mode = MODE_FUNCTIONS;
                i -= 8;
                continue;
            }
            printf("variable: name @ string %d, type @ string %d\n", name, type);
        }
        if(mode == MODE_FUNCTIONS) {
            if(!valid_function_header) {
                unsigned int len = strlen(SECTION_HEADER_FUNCTIONS);
                char *section_header = malloc((len + 1) * sizeof(char));
                for(unsigned int j = 0; j <= len; j++) {
                    section_header[j] = buffer[i + j];
                }
                i += len;
                section_header[len] = '\0';
                DEBUG("header: %s\n", section_header)
                if(strncmp(section_header, SECTION_HEADER_FUNCTIONS, strlen(SECTION_HEADER_FUNCTIONS)) != 0) {
                    fprintf(stderr, RED "'%s' isn't a valid string section header! (expected '%s')\n" RESET, 
                            section_header, SECTION_HEADER_FUNCTIONS);
                    free(section_header);
                    goto end;
                }
                valid_function_header = true;
                free(section_header);
            }
            uint32_t name = ((uint32_t) buffer[i    ] << 24) |
                            ((uint32_t) buffer[i + 1] << 16) |
                            ((uint32_t) buffer[i + 2] << 8)  |
                            ((uint32_t) buffer[i + 3]);
            i += 4; // Move past int
            uint32_t signature = ((uint32_t) buffer[i    ] << 24) |
                                 ((uint32_t) buffer[i + 1] << 16) |
                                 ((uint32_t) buffer[i + 2] << 8)  |
                                 ((uint32_t) buffer[i + 3]);
            i += 4; // Move past int
            uint32_t length = ((uint32_t) buffer[i    ] << 24) |
                              ((uint32_t) buffer[i + 1] << 16) |
                              ((uint32_t) buffer[i + 2] << 8)  |
                              ((uint32_t) buffer[i + 3]);
            i += 4; // Move past int
            
            //            . d a t                       a
            if(name == 0x2E646174 && (signature >> 24) == 0x61) {
                // Need to undo the changes to i
                DEBUG(".functions end\n")
                mode = DONE;
                i -= 8;
                continue;
            }
            printf("function: name @ string %d, signature @ string %d\n", name, signature);
            const int j = i + length;
            for(; i < j; i++) {
                printf("opcode: 0x%x (%s)\n", buffer[i], opcode_name_table[buffer[i]]);
            }
            --i; // Fix i because i++ at the end of the loop
        }
    }

end:
    free(buffer); // calloc()
    return 0;
}

