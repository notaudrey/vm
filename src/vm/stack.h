#pragma once

#include "../common/config.h"

struct stack_s {
    void *stack[STACK_LIMIT];
    int current_position;
};

struct stack_s *stack_new(void);
void stack_push(struct stack_s *, void *);
void *stack_pop(struct stack_s *);
