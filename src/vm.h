#pragma once

// See https://github.com/munificent/wren/blob/master/src/vm/wren_vm.h
enum opcode_e {
    #define OPCODE(name, _) OPCODE_##name,
    #include "opcodes.h"
    #undef OPCODE
};

struct vm_s {
    struct stack_s *stack;
    unsigned int opcode_counter;
    // The opcode dispatch table is just an array of pointers to void functions
    // declared in vm.c, which we use to actually handle opcode dispatch. The
    // vm simply has to do something like 
    //   *(opcode_dispatch_table + opcode_number)(vm)
    // to call the proper handler function for every opcode.
    void **opcode_dispatch_table;
};

struct vm_s *init_vm(void);

void destroy_vm(struct vm_s *);

