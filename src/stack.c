#include <stdlib.h>

#include "config.h"
#include "stack.h"

void push(struct stack_s *stack, const int value) {
    stack->stack[stack->current_position] = value;
    stack->current_position += 1;
}

int pop(struct stack_s *stack) {
    int value = stack->stack[stack->current_position];
    stack->current_position -= 1;
    return value;
}
