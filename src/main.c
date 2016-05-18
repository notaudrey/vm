#include <stdio.h>

#include "config.h"
#include "debug.h"
#include "stack.h"
#include "value.h"
#include "vm.h"

// Generated by make
#include "version.h"

// Create a lookup table so that we can see opcode names during debugging.
static char *opcode_name_table[] = {
// Abuse of stringification
#define OPCODE(name, _) "" #name "",
#include "opcodes.h"
#undef OPCODE
};

char *error_lookup_table[] = {
    #define QUOTE(x) #x
    #define ERROR(name) QUOTE(name),
    #include "error.h"
    #undef ERROR
    #undef QUOTE
};


int main(int argc, char** argv) {
    printf("git short hash: %s\n", GIT_SHORT_HASH);
    printf("git long hash: %s\n", GIT_LONG_HASH);
    printf("build date: %s\n", DATE);

    struct vm_s *vm = vm_init();
    enum opcode_e program[] = {
        OPCODE_TRUE,
        OPCODE_FALSE,
        OPCODE_POP,
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
        //                                                                          |
        //          => cast to a bool(struct < => get value at the opcode's index <=| 
        //          |  vm_s *) function ptr  | |  in the opcode dispatch table     || => call function with vm as arg
        //          |                        | |                                   || |  |
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
    vm_destroy(vm);
    return 0;
}

