/// bytecode.h
/// 
/// Describes how the bytecode in a file is formatted. While it isn't the 
/// format parsing mechanism(s) that is stored in here, the contents of this
/// header will still be helpful in understanding both the layout of a bytecode
/// file and for parsing such a file.

#pragma once

#include <stdint.h>

/// The magic number that starts every single bytecode file. If a file doesn't 
/// start with this sequence of bytes, it is assumed to be an invalid file and 
/// is therefore going to cause the VM to error out.
const uint64_t MAGIC_NUMBER = 0xDEADBEEFBABECAFE;

/// The "header" for the section of the bytecode that is just a string lookup
/// table. This should literally be the string ".strings" (hex values listed 
/// below). After this string, every 3 bytes will be used for string lookup and
/// storage. The first byte will be the string id (starting at 0). The second 
/// byte is the offset into the bytecode of where the string is located, and
/// the third byte is the length of the string. For example:
/// 
/// 
/// 2e 73 74 72 69 6e 67 73     # .strings
/// 00 2b 04                    # string 0 starts at 2b and has length 0x04
/// 01 2f 0f                    # string 1 starts at 2f and has length 0x0f
/// 
/// # .functions etc goes here
/// 
/// 2e 2e 2e 2e                 # string 0, at 0x2b, with length 4, contents 
///                             # '....'
/// 
/// # rest of strings and other data
/// 
/// EOF
const char *SECTION_HEADER_STRINGS = ".strings";

/// The "header" for the section of the bytecode that is just for function 
/// definitions. Function definitions start with opcode 0x03
/// (OPCODE_FUNCTION_DECLARATOR). The next two bytes are ids for the strings
/// that represent the function's name and signature, respectively. The four
/// bytes after that are an integer representing how long the function is, in
/// bytes (because honestly, if you need more than ~4B bytes for a function, 
/// you're doing it wrong...). After that, every byte is interpreted as an
/// opcode or opcode argument, up until the end of the function. For example:
/// 
///     .strings
///     00 XX 04 # function name string
///     01 XX 03 # function signature string
///     
///     # ...
///     
///     .functions
///     # Declares a function with name at string 0 and signature at string 1
///     # Effectively declares a function with name 'test' and signature '()V', ie.
///     # a function named 'test' that takes no arguments and returns nothing, with
///     # a function body of:
///     # 
///     # 0x00 # TRUE
///     # 0x01 # FALSE
///     # 0x02 # POP
///     # 0x02 # POP
///     # 
///     # which literally only pushes two values onto the stack and then 
///     # immediately pops them off.
///     03 00 01 00 01 02 02
///     
///     # ...
///     
///     00 74 65 73 74 # string 0: test
///     01 28 29 56    # string 1: ()V
/// 
/// Once a function declaration ends (based on the offset after the 3 bytes),
/// functions may continue to be declared infinitely (so to speak). Once all
/// function declarations are finished, the constant data section is implicitly
/// started, and no header is needed therefor.
const char *SECTION_HEADER_FUNCTIONS = ".functions";

