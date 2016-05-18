#include <stdlib.h>

#include "../common/config.h"
#include "stack.h"
#include "value.h"

struct stack_s *stack_new(void) {
    struct stack_s *stack = malloc(sizeof(struct stack_s));
    stack->current_position = 0;
    return stack;
}

void stack_push(struct stack_s *stack, void *value) {
    stack->stack[stack->current_position] = value;
    stack->current_position += 1;
}

void *stack_pop(struct stack_s *stack) {
    stack->current_position -= 1;
    struct value_s *value = stack->stack[stack->current_position];
    stack->stack[stack->current_position] = NULL;
    return value;
}
