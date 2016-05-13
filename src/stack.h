#pragma once
#include "config.h"

struct stack_s {
    int stack[STACK_LIMIT];
    int current_position;
};

void push(struct stack_s *, int);
int pop(struct stack_s *);
