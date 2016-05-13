#include <stdio.h>

#include "config.h"
#include "stack.h"

// See https://github.com/munificent/wren/blob/master/src/vm/wren_vm.h
enum opcode_e {
    #define OPCODE(name, _) OPCODE_##name,
    #include "opcodes.h"
    #undef OPCODE
};

int main(int argc, char** argv) {
    enum opcode_e program[2] = {
        OPCODE_TRUE,
        OPCODE_POP
    };
}
