/// The bytecode parser for the VM

#include <stdlib.h>
#include <stdio.h>
#include <bsd/string.h>

#include "vm.h"
#include "../common/debug.h"
#include "../common/bytecode.h"

enum mode_e {
    NONE,
    MODE_STRINGS,
    MODE_VARIABLES,
    MODE_FUNCTIONS,
    DONE,
};

// Create a lookup table so that we can see opcode names during debugging.
void parse_bytecode(char *file_name, struct vm_s *vm) {
    DEBUG("file: %s\n", file_name)
    FILE *file = fopen(file_name, "rb");
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
        fprintf(stderr, RED "not valid bytecode: %s\n" RESET, file_name);
        fprintf(stderr, RED "magic:    0x%lx\n" RESET, magic_number);
        fprintf(stderr, RED "expected: 0x%lx\n" RESET, MAGIC_NUMBER);
        goto end;
    }

    enum mode_e mode = NONE;
    // Read to the end of the buffer
    int string_counter = 0;
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
            char string[length+1];
            // So linting throws a warning on 'buffer + location' as it is 
            // being passed to strlcpy. However, this SEEMS to work fine, so
            // I'm not quite brave enough to change it for the sake of changing
            // it. (. _ . )
            strlcpy(string, buffer + location, length + 1);
            string[length] = '\0';
            DEBUG("Copied from buffer: %s\n", string);
            struct vm_constant_string_s *constant_string = malloc(sizeof(struct vm_constant_string_s));

            constant_string->string_namespace = malloc(sizeof(char) * strlen(file_name));
            strlcpy(constant_string->string_namespace, file_name, strlen(file_name) + 1);

            constant_string->string_id = string_counter++;

            constant_string->string_value = malloc(sizeof(char) * strlen(string));
            strlcpy(constant_string->string_value, string, strlen(string) + 1);
            vm_add_string(vm, constant_string);
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
            struct vm_variable_s *variable = malloc(sizeof(struct vm_variable_s));

            variable->variable_namespace = malloc(sizeof(char) * strlen(file_name));
            strlcpy(variable->variable_namespace, file_name, strlen(file_name) + 1);

            struct vm_constant_string_s *name_string = (struct vm_constant_string_s *) list_get(vm->strings, name);
            struct vm_constant_string_s *type_string = (struct vm_constant_string_s *) list_get(vm->strings, type);

            variable->variable_name = malloc(sizeof(char) * strlen(name_string->string_value));
            strlcpy(variable->variable_name, name_string->string_value, strlen(name_string->string_value) + 1);

            variable->variable_type = malloc(sizeof(char) * strlen(type_string->string_value));
            strlcpy(variable->variable_type, type_string->string_value, strlen(type_string->string_value) + 1);

            vm_add_variable(vm, variable);
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
            //struct vm_function_s *function = malloc(sizeof(struct vm_function_s));
            
            struct vm_function_s *function = malloc(sizeof(struct vm_function_s));

            function->function_namespace = malloc(sizeof(char) * strlen(file_name));
            strlcpy(function->function_namespace, file_name, strlen(file_name) + 1);

            struct vm_constant_string_s *name_string = (struct vm_constant_string_s *) list_get(vm->strings, name);
            struct vm_constant_string_s *type_string = (struct vm_constant_string_s *) list_get(vm->strings, signature);

            function->function_name = malloc(sizeof(char) * strlen(name_string->string_value));
            strlcpy(function->function_name, name_string->string_value, strlen(name_string->string_value) + 1);
            function->function_signature = malloc(sizeof(char) * strlen(type_string->string_value));
            strlcpy(function->function_signature, type_string->string_value, strlen(type_string->string_value) + 1);
            function->function_body = malloc(sizeof(uint32_t) * length);
            
            const int j = i + length;
            // TODO: handle load/store/invoke opcodes!!
            for(; i < j; i++) {
                function->function_body[i] = buffer[i];
                if(buffer[i] == OPCODE_LOAD_LOCAL || buffer[i] == OPCODE_STORE_LOCAL
                         || buffer[i] == OPCODE_LOAD_GLOBAL || buffer[i] == OPCODE_STORE_GLOBAL
                         || buffer[i] == OPCODE_INVOKE_FUNCTION) {
                    uint32_t parameter_one = ((uint32_t) buffer[i + 1] << 24) |
                                             ((uint32_t) buffer[i + 2] << 16) |
                                             ((uint32_t) buffer[i + 3] << 8)  |
                                             ((uint32_t) buffer[i + 4]);
                    function->function_body[i] = parameter_one;
                    i += 4;
                    uint32_t parameter_two = ((uint32_t) buffer[i + 1] << 24) |
                                             ((uint32_t) buffer[i + 2] << 16) |
                                             ((uint32_t) buffer[i + 3] << 8)  |
                                             ((uint32_t) buffer[i + 4]);
                    function->function_body[i] = parameter_two;
                    i += 4;
                }
            }
            vm_add_function(vm, function);
            --i; // Fix i because i++ at the end of the loop
        }
    }

end:
    free(buffer); // calloc()
    // TODO: Check errors, stuff everything into *vm
}
