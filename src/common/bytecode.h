/// bytecode.h
/// 
/// Describes how the bytecode in a file is formatted. While it isn't the 
/// format parsing mechanism(s) that is stored in here, the contents of this
/// header will still be helpful in understanding both the layout of a bytecode
/// file and for parsing such a file.

#pragma once

#include <stdint.h>

// See https://github.com/munificent/wren/blob/master/src/vm/wren_vm.h
enum opcode_e {
    #define OPCODE(name, _) OPCODE_##name,
    #include "../common/opcodes.h"
    #undef OPCODE
};

enum error_e {
    #define ERROR(name) ERROR_##name,
    #include "../common/error.h"
    #undef ERROR
};

const extern uint64_t MAGIC_NUMBER;
const extern char *SECTION_HEADER_STRINGS;
const extern char *SECTION_HEADER_VARIABLES;
const extern char *SECTION_HEADER_FUNCTIONS;
const extern char *SECTION_HEADER_DATA;
