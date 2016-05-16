#include <stdlib.h>

#include "config.h"
#include "stack.h"
#include "value.h"

void push(struct stack_s *stack, struct value_s *value) {
    stack->stack[stack->current_position] = value;
    stack->current_position += 1;
}

struct value_s *pop(struct stack_s *stack) {
    struct value_s *value = stack->stack[stack->current_position];
    stack->current_position -= 1;
    return value;
}
