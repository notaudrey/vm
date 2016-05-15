#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "stack.h"
#include "version.h"
#include "vm.h"

void dispatch_opcode_TRUE(struct vm_s *vm);

void dispatch_opcode_POP(struct vm_s *vm);

struct vm_s *init_vm(void) {
    struct vm_s *vm = (struct vm_s *) malloc(sizeof(struct vm_s));
    vm->stack = (struct stack_s *) malloc(sizeof(struct stack_s));
    vm->stack->current_position = 0;
    vm->opcode_counter = 0;
    vm->opcode_dispatch_table = (void **) malloc(sizeof(void*) * (unsigned long)(OPCODE_COUNT - '0'));
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

void destroy_vm(struct vm_s *vm) {
    free(vm->stack);
    free(vm->opcode_dispatch_table);
    free(vm);
}

void dispatch_opcode_TRUE(struct vm_s *vm) {
    push(vm->stack, 1);
}

void dispatch_opcode_POP(struct vm_s *vm) {
    pop(vm->stack);
}

