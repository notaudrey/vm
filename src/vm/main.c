#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../common/config.h"
#include "../common/debug.h"
#include "stack.h"
#include "value.h"
#include "vm.h"

// Generated by make
#include "../common/version.h"

// Create a lookup table so that we can see opcode names during debugging.
static char *opcode_name_table[] = {
// Abuse of stringification
#define OPCODE(name, _) "" #name "",
#include "../common/opcodes.h"
#undef OPCODE
};

char *error_lookup_table[] = {
    #define QUOTE(x) #x
    #define ERROR(name) QUOTE(name),
    #include "../common/error.h"
    #undef ERROR
    #undef QUOTE
};

const uint64_t MAGIC_NUMBER = 0xDEADBEEFBABECAFE;

int main(int argc, char** argv) {
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
        exit(1);
    } 
#ifdef __DEBUG_BUILD
    else {
        DEBUG("valid bytecode!\n")
    }
#endif

    char section_header[] = {
        buffer[8],
        buffer[9],
        buffer[10],
        buffer[11],
        buffer[12],
        buffer[13],
        buffer[14],
        buffer[15],
    };
    DEBUG("header: '%s'\n", section_header)

    struct vm_s *vm = vm_init();
    enum opcode_e program[] = {
        OPCODE_TRUE,
        OPCODE_FALSE,
        OPCODE_POP,
        OPCODE_POP,
    };

    // Eventually this won't matter
    const int program_length = sizeof(program) / sizeof(*program);
    DEBUG("program length: %d\n", program_length);
#ifdef __DEBUG_BUILD
    DEBUG("program:\n")
    DEBUG("--------\n")
    for(int i = 0; i < program_length; i++) {
        DEBUG("0x%08lx: 0x%x/%s\t#%d\n", i * sizeof(enum opcode_e), program[i], opcode_name_table[program[i]], i)
    }
    DEBUG("--------\n")
#endif
    
    for(int i = 0; i < program_length; i++) {
        enum opcode_e opcode = program[i];
        DEBUG("opcode: 0x%x (%s)\n", opcode, opcode_name_table[opcode]);
        
        //                                                            end typecast <=
        //          => cast to a bool(struct < => get value at the opcode's index <=| 
        //          |  vm_s *) function ptr  | |  in the opcode dispatch table     || => call function with vm as arg |
        //          |                        | |                                   || |  |----------------------------|
        int error = ((int (*) (struct vm_s *)) *(vm->opcode_dispatch_table + opcode)) (vm);
        // No negative error codes allowed.
        if(error > 0) {
            fprintf(stderr, RED "[ERROR] Failed on opcode 0x%x/%s (#%d, 0x%08lx), bailing out...\n", opcode, 
                    opcode_name_table[opcode], vm->opcode_counter, vm->opcode_counter * sizeof(enum opcode_e));
            fprintf(stderr, RED "[ERROR] Error: %s\n", error_lookup_table[error]);
            goto end;
        }
        ++vm->opcode_counter;
#ifdef __DEBUG_BUILD
        // We do this inside of an #ifdef because generating a string for this
        // and having to deal with all that nonsense would be a pain.
        DEBUG(WHT "stack: {");
        struct stackframe_s *frame = vm_peek_frame(vm);
        for(int i = 0; i < frame->stack->current_position; i++) {
            struct value_s *value = frame->stack->stack[i];
            switch(value->type) {
                case BOOLEAN:
                    printf(RESET "%s", value->bool_value ? "true" : "false");
                    break;
                case INT:
                    printf(RESET "%d", value->int_value);
                    break;
                case FLOAT:
                    printf(RESET "%f", value->float_value);
                    break;
                default:
                    fprintf(stderr, RED "Failed printing stack value @ stack[%d] (%p), bailing out..." RESET "\n", 
                            i, (void *) value);
                    goto end;
            }
            if(i < frame->stack->current_position - 1) {
                printf(WHT ", ");
            }
        }
        printf(WHT "}" RESET "\n");
#endif
    }
end:
    free(buffer);
    vm_destroy(vm);
    return 0;
}

