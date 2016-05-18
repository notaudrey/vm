#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "debug.h"
#include "stack.h"
#include "value.h"
#include "version.h"
#include "vm.h"

#define OPCODE(name, _) int dispatch_opcode_##name(struct vm_s *);
#include "opcodes.h"
#undef OPCODE
static inline bool vm_check_stack_overflow(struct vm_s *);
static inline bool vm_check_stack_underflow(struct vm_s *);
static inline bool vm_check_frame_stack_underflow(struct vm_s *);
static inline bool vm_check_frame_stack_overflow(struct vm_s *);

struct vm_s *vm_init(void) {
    struct vm_s *vm = malloc(sizeof(struct vm_s));
    vm->stack = stack_new();
    vm->opcode_counter = 0;

    // bool (*opcode_dispatch_table[]) (struct vm_s *);
    vm->opcode_dispatch_table = malloc(sizeof(int *) * (unsigned long)(OPCODE_COUNT - '0'));
    unsigned int counter = 0;

    // Define the OPCODE macro as an update to the dispatch table, where every
    // opcode is used to declare a pointer to the proper handler function (by
    // opcode name) and store it in the array that the vm will then be able to
    // use to handle opcode dispatch
    #define OPCODE(name, _) *(vm->opcode_dispatch_table + counter) = *dispatch_opcode_##name; \
    ++counter;
    #include "opcodes.h"
    #undef OPCODE

    // TODO: Temporary
    vm_push_frame(vm, vm_create_frame("test_frame", 0));
    return vm;
}

void vm_destroy(struct vm_s *vm) {
    while(vm->stack->current_position != 0) {
        struct stackframe_s *frame = vm_pop_frame(vm);
        while(frame->stack->current_position != 0) {
            struct value_s *value = stack_pop(frame->stack);
            free(value);
        }
        while(frame->locals->size != 0) {
            struct value_s *value = list_remove(frame->locals, frame->locals->size - 1);
            free(value);
        }
        free(frame->context);
        free(frame->locals->data);
        free(frame->locals);
        free(frame->stack);
        free(frame);
    }
    free(vm->stack);
    free(vm->opcode_dispatch_table);
    free(vm);
}

struct stackframe_s *vm_create_frame(char *frame_name, unsigned int line) {
    struct stackframe_context_s *context = malloc(sizeof(struct stackframe_context_s));
    context->frame_name = frame_name;
    context->current_line = line;
    struct stackframe_s *frame = malloc(sizeof(struct stackframe_s));
    frame->context = context;
    frame->stack = stack_new();
    frame->locals = list_new(8);
    DEBUG("Created new frame: \"%s\", line %u\n", frame_name, line)
    return frame;
}

bool vm_push_frame(struct vm_s *vm, struct stackframe_s *frame) {
    if(vm_check_stack_overflow(vm)) {
        return false;
    }
    stack_push(vm->stack, frame);
    DEBUG("Pushed frame: %s (line %u)\n", frame->context->frame_name, frame->context->current_line)
    return true;
}

struct stackframe_s *vm_pop_frame(struct vm_s *vm) {
    if(vm_check_stack_underflow(vm)) {
        // Do something???
    }
#ifdef __DEBUG_BUILD
    struct stackframe_s *frame = stack_pop(vm->stack);
    DEBUG("Popped frame: %s (line %u)\n", frame->context->frame_name, frame->context->current_line)
    return frame;
#else
    return stack_pop(vm->stack);
#endif
}

/// 
/// Opcode dispatch functions
/// TODO: Check stack over/underflow
/// 

int dispatch_opcode_TRUE(struct vm_s *vm) {
    if(vm_check_frame_stack_overflow(vm)) {
        return ERROR_STACK_OVERFLOW;
    }
    // Right now, naively allocate something new on the stack every time
    struct value_s *value = malloc(sizeof(struct value_s));
    DEBUG("malloc'd: %p\n", (void *) value)
    value->type = BOOLEAN;
    value->bool_value = true;
    stack_push(vm_peek_frame(vm)->stack, value);
    return ERROR_NO_ERROR;
}

int dispatch_opcode_FALSE(struct vm_s *vm) {
    if(vm_check_frame_stack_overflow(vm)) {
        return ERROR_STACK_OVERFLOW;
    }
    // Right now, naively allocate something new on the stack every time
    struct value_s *value = malloc(sizeof(struct value_s));
    DEBUG("malloc'd: %p\n", (void *) value)
    value->type = BOOLEAN;
    value->bool_value = false;
    stack_push(vm_peek_frame(vm)->stack, value);
    return ERROR_NO_ERROR;
}

int dispatch_opcode_POP(struct vm_s *vm) {
    if(vm_check_frame_stack_underflow(vm)) {
        return ERROR_STACK_UNDERFLOW;
    }
    struct value_s *popped = stack_pop(vm_peek_frame(vm)->stack);
    // Because for some reason, &popped gives nonsense but (void *) popped works fine...
    DEBUG("free'd: %p\n", (void *) popped);
    // Right now, naively free anything popped from the stack
    free(popped);
    return ERROR_NO_ERROR;
}

/// 
/// Helper functions. Checking for over/underflow, peeking at the top 
/// stackframe, ...
/// 

struct stackframe_s *vm_peek_frame(struct vm_s *vm) {
    return (struct stackframe_s *) vm->stack->stack[vm->stack->current_position - 1];
}

/// Check for stack overflow, ie stack head is at the limit
static inline bool vm_check_stack_overflow(struct vm_s *vm) {
    return vm->stack->current_position == STACK_LIMIT;
}

/// Check for stack underflow, ie stack head is at the bottom
static inline bool vm_check_stack_underflow(struct vm_s *vm) {
    return vm->stack->current_position == 0;
}

/// Check for stack overflow, ie stack head is at the limit
static inline bool vm_check_frame_stack_overflow(struct vm_s *vm) {
    return vm_peek_frame(vm)->stack->current_position == STACK_LIMIT;
}

/// Check for stack underflow, ie stack head is at the bottom
static inline bool vm_check_frame_stack_underflow(struct vm_s *vm) {
    return vm_peek_frame(vm)->stack->current_position == 0;
}

