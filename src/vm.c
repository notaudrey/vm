#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "debug.h"
#include "stack.h"
#include "value.h"
#include "version.h"
#include "vm.h"

bool dispatch_opcode_TRUE(struct vm_s *);
bool dispatch_opcode_FALSE(struct vm_s *);
bool dispatch_opcode_POP(struct vm_s *);
static inline bool vm_check_stack_overflow(struct vm_s *vm);
static inline bool vm_check_stack_underflow(struct vm_s *vm);

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
    DEBUG("INFO", "pre-checks\n")
    if(vm_check_stack_overflow(vm)) {
        return false;
    }
    DEBUG("INFO", "malloc\n")
    // Right now, naively allocate something new on the stack every time
    struct value_s *value = (struct value_s *) malloc(sizeof(struct value_s));
    DEBUG("INFO", "malloc'd: %p\n", (void *) value)
    DEBUG("INFO", "type\n")
    value->type = BOOLEAN;
    DEBUG("INFO", "value\n")
    value->bool_value = true;
    DEBUG("INFO", "push\n")
    push(vm->stack, value);
    DEBUG("INFO", "ret\n")
    return true;
}

bool dispatch_opcode_FALSE(struct vm_s *vm) {
    if(vm_check_stack_overflow(vm)) {
        return false;
    }
    // Right now, naively allocate something new on the stack every time
    struct value_s *value = (struct value_s *) malloc(sizeof(struct value_s));
    value->type = BOOLEAN;
    value->bool_value = false;
    push(vm->stack, value);
    return true;
}

bool dispatch_opcode_POP(struct vm_s *vm) {
    DEBUG("INFO", "pre-checks\n")
    if(vm_check_stack_underflow(vm)) {
        return false;
    }
    DEBUG("INFO", "pop\n")
    struct value_s *popped = pop(vm->stack);
    // Right now, naively free anything popped from the stack
    DEBUG("INFO", "free\n")
    free(popped);
    DEBUG("INFO", "done\n")
    return true;
}

/// Check for stack overflow, ie stack head is at the limit
static inline bool vm_check_stack_overflow(struct vm_s *vm) {
    return vm->stack->current_position == STACK_LIMIT;
}

/// Check for stack underflow, ie stack head is at the bottom
static inline bool vm_check_stack_underflow(struct vm_s *vm) {
    return vm->stack->current_position == 0;
}
