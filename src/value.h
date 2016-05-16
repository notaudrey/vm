#pragma once

#include <stdbool.h>

enum value_type_e {
    INT,
    BOOLEAN,
    FLOAT,
};

struct value_s {
    enum value_type_e type;
    union {
        int int_value;
        bool bool_value;
        float float_value;
    };
};
