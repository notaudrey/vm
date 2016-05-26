#pragma once

#include <stdbool.h>
#include "vm.h"

enum value_type_e {
    INT,
    BOOLEAN,
    FLOAT,
    FUNCTION,
};

struct value_s {
    enum value_type_e type;
    union {
        int int_value;
        bool bool_value;
        float float_value;
        struct vm_function_s *function_value;
    };
};

struct vm_function_s {
    char *function_namespace;
    char *function_name;
    char *function_signature;
    enum opcode_e function_body[];
};

struct vm_variable_s {
    char *variable_namespace;
    char *variable_name;
    char *variable_type;
    struct value_s value;
};

