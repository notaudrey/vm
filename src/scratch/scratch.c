#include <stdlib.h>
#include <stdio.h>

#include "../common/bytecode.h"
//#include "../common/opcodes.h"
//#include "../common/error.h"
#include "../common/debug.h"
#include "../common/version.h"

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
    buffer = calloc((size + 1), sizeof(char));
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

    // Read to the end of the buffer
    for(int i = 8; i < size; i++) {
        char *section_header = malloc((strlen(SECTION_HEADER_STRINGS) + 1) * sizeof(char));
        for(int i = 8; i <= 15; i++) {
            section_header[i - 8] = buffer[i];
        }
        section_header[8] = '\0';
        DEBUG("header: '%s'\n", section_header)
        if(strncmp(section_header, SECTION_HEADER_STRINGS, strlen(SECTION_HEADER_STRINGS)) != 0) {
            fprintf(stderr, RED "'%s' isn't a valid string section header! (expected '%s')" RESET, 
                    section_header, SECTION_HEADER_STRINGS);
            goto end;
        }
        free(section_header);
    }

end:
    free(buffer); // calloc(); :32
    return 0;
}

