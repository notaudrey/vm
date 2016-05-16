#pragma once

#include "config.h"
#include "value.h"

struct stack_s {
    struct value_s *stack[STACK_LIMIT];
    int current_position;
};

void push(struct stack_s *, struct value_s *);
struct value_s *pop(struct stack_s *);
