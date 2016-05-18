#pragma once

#include <stdbool.h>

#include "../common/list.h"
#include "stack.h"

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

struct vm_s {
    struct stack_s *stack;
    unsigned int opcode_counter;
    // The opcode dispatch table is just an array of pointers to void functions
    // declared in vm.c, which we use to actually handle opcode dispatch. The
    // vm simply has to do something like 
    //   *(opcode_dispatch_table + opcode_number)(vm)
    // to call the proper handler function for every opcode. Opcode dispatch
    // functions return integers, similar to the return value of main(). A
    // return value of 0 means that the function call succeeded. Any other
    // return value is an offset into the error message lookup table in error.h
    // and will be printed out, and the VM will exit immediately.
    int (**opcode_dispatch_table) (struct vm_s *);
};

/// The context for a stackframe. Would be used for printing out information
/// when unwinding the stack to print out "oh noes an error happened! ;-;" or
/// similar.
struct stackframe_context_s {
    /// The name of the function that 'owns' this stack frame. As each 
    /// successive function is called, a stack frame is allocated for that
    /// specific function, giving it its own 'storage' for local variables, 
    /// function-specific stack, etc.
    char* frame_name;
    /// The current line being executed in this function. Meant for debugging. 
    /// TODO: Make an opcode for this...
    unsigned int current_line;
};

/// A single stackframe is the "context" of a given function, and cannot be 
/// accessed from within any other stack frames. Each stack frame has its own
/// local stack and local variables. 
///
/// TODO: Determine how returning from a function should work
struct stackframe_s {
    struct stackframe_context_s *context;
    /// Local variables of this stack frame. This like a 'this' pointer would 
    /// also go here
    struct list_s *locals;
    /// Stack for this stack frame. Used exactly as you would think.
    struct stack_s *stack;
};

struct vm_s *vm_init(void);
void vm_destroy(struct vm_s *);
bool vm_push_frame(struct vm_s *, struct stackframe_s *);
struct stackframe_s *vm_pop_frame(struct vm_s *);
struct stackframe_s *vm_create_frame(char *, unsigned int);
struct stackframe_s *vm_peek_frame(struct vm_s *);

