// We don't do this because we need to be able to include this multple times in
// order to be able to abuse macros in certain ways c:
//#pragma once

// The first argument is the name of the opcode. The second argument is what
// effect the opcode has on the vm's stack.
// TODO: Do something with second argument

/// Pushes the boolean value "true" onto the stack
OPCODE(TRUE, 1)

/// Pushes the boolean value "false" onto the stack
OPCODE(FALSE, 1)

/// Pops the top value off of the stack
OPCODE(POP, -1)

/// Returns a value from a function. Pops an entire frame off of the main stack
/// and sets the return value stored elsewhere in the vm
OPCODE(RETURN, -1)

/// Loads the global variable at the index specified by the following int (4 
/// bytes)
OPCODE(LOAD_GLOBAL, 1)

/// Stores the top of the stack in the global variable at the index specified
/// by the following int (4 bytes)
OPCODE(STORE_GLOBAL, -1)

/// Like LOAD_GLOBAL, but for local variables
OPCODE(LOAD_LOCAL, 1)

/// Like STORE_GLOBAL, but for local variables
OPCODE(STORE_LOCAL, -1)

/// Invoke a function. Currently takes 2 arguments: name, signature. Both of
/// these are the two 4-byte sequences that follow this opcode
OPCODE(INVOKE_FUNCTION, -999)
