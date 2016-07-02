#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "../common/bytecode.h"

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
    uint32_t *function_body;
};

struct vm_variable_s {
    char *variable_namespace;
    char *variable_name;
    char *variable_type;
    struct value_s value;
};

struct vm_constant_string_s {
    char *string_namespace;
    unsigned int string_id;
    char *string_value;
};

