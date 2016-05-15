#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "stack.h"
#include "version.h"
#include "vm.h"

bool dispatch_opcode_TRUE(struct vm_s *);
bool dispatch_opcode_FALSE(struct vm_s *);
bool dispatch_opcode_POP(struct vm_s *);

struct vm_s *vm_init(void) {
    struct vm_s *vm = (struct vm_s *) malloc(sizeof(struct vm_s));
    vm->stack = (struct stack_s *) malloc(sizeof(struct stack_s));
    vm->stack->current_position = 0;
    vm->opcode_counter = 0;
    // bool (*opcode_dispatch_table[]) (struct vm_s *);
    vm->opcode_dispatch_table = (bool (**) (struct vm_s *)) malloc(sizeof(bool *) * (unsigned long)(OPCODE_COUNT - '0'));
    unsigned int counter = 0;
    // Define the OPCODE macro as an update to the dispatch table, where every
    // opcode is used to declare a pointer to the proper handler function (by
    // opcode name) and store it in the array that the vm will then be able to
    // use to handle opcode dispatch
    #define OPCODE(name, _) *(vm->opcode_dispatch_table + counter) = *dispatch_opcode_##name; \
    ++counter;
    #include "opcodes.h"
    #undef OPCODE
    return vm;
}

void vm_destroy(struct vm_s *vm) {
    free(vm->stack);
    free(vm->opcode_dispatch_table);
    free(vm);
}

bool dispatch_opcode_TRUE(struct vm_s *vm) {
    if(vm->stack->current_position == STACK_LIMIT) {
        return false;
    }
    push(vm->stack, 1);
    return true;
}

bool dispatch_opcode_FALSE(struct vm_s *vm) {
    if(vm->stack->current_position == STACK_LIMIT) {
        return false;
    }
    push(vm->stack, 0);
    return true;
}

bool dispatch_opcode_POP(struct vm_s *vm) {
    if(vm->stack->current_position == 0) {
        return false;
    }
    pop(vm->stack);
    return true;
}

