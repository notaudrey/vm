#pragma once

#include <stdbool.h>

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
    bool (**opcode_dispatch_table) (struct vm_s *);
};

/// The context for a stackframe. Would be used for printing out information
/// when unwinding the stack to print out "oh noes an error happened! ;-;" or
/// similar.
struct stackframe_context_s {
    ;
};

/// A single stackframe is the "context" of a given function, and cannot be 
/// accessed from within any other stack frames. Each stack frame has its own
/// local stack and local variables. 
///
/// TODO: Determine how returning from a function should work
struct stackframe_s {
    /// Local variables of this stack frame. This like a 'this' pointer would 
    /// also go here
    /// TODO: Oh right that's why I needed a linked list...
    struct stack_s *locals;
    /// Stack for this stack frame. Used exactly as you would think.
    struct stack_s *stack;
};

struct vm_s *vm_init(void);

void vm_destroy(struct vm_s *);

