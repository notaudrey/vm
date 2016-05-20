// We don't do this because we need to be able to include this multple times in
// order to be able to abuse macros in certain ways c:
//#pragma once

// The first argument is the name of the opcode. The second argument is what
// effect the opcode has on the vm's stack.
// TODO: Do something with second argument
OPCODE(TRUE, 1)
OPCODE(FALSE, 1)
OPCODE(POP, -1)
OPCODE(VARIABLE_DECLARATOR, 0)
OPCODE(FUNCTION_DECLARATOR, 0)

