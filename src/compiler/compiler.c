#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    FILE* file = fopen("test.adri", "wb");
    unsigned char data[] = {
        // File header 
        // -----------
        // 0xDEADBEEFBABECAFE
        // Yes, I was trying to cram 4 'words' into 8 bytes.
        0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xBE, 0xCA, 0xFE,

        // Section header
        // --------------
        // First section is string lookup table
        '.', 's', 't', 'r', 'i', 'n', 'g', 's',
        // String start, length. Start and length are in bytes.
        // Start is the 'address' of the string's first character in the file.
        // Length is just how many bytes the string is.
        // 
        // Our test string is 'test', hence size=0x04
        //
        // => Starts at 0x55<=
        // |                 |  => Length of 0x04 <=--
        0x00, 0x00, 0x00, 0x59, 0x00, 0x00, 0x00, 0x04, 
        0x00, 0x00, 0x00, 0x5D, 0x00, 0x00, 0x00, 0x03,
        0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x01,
        
        // Section header
        // --------------
        // Second section is variables
        '.', 'v', 'a', 'r', 'i', 'a', 'b', 'l', 'e', 's',
        // Variable declaration
        // --------------------
        // Declare a variable with name of string 0x00000000 and type of string 
        // 0x00000002. In this example file, string 0x00000000 is 'test' and
        // string 0x00000002 is 'I', so this would be declaring a variable
        // 'test' of type 'integer'
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,

        // Section header
        // --------------
        // Third section is functions
        '.', 'f', 'u', 'n', 'c', 't', 'i', 'o', 'n', 's',

        // Function header 
        // ---------------
        // 0x04 is the function declaration opcode. The next two bytes are 
        // string identifiers. The first string is the function name, and the
        // second string is the function signature. 
        // The next four bytes are an integer representing the length of the
        // function, in bytes.
        // 
        // => Function name: string 0x00 00 00 00
        // |                       => Function signature: string 0x00 00 00 01
        // |                       |                       => Length of this function: 0x00 00 00 04
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04,
        // Function body
        // -------------
        // Every byte in here, up to the end of the function, is interpreted as
        // one of the opcodes from opcodes.h
        //
        // This is just
        //   TRUE
        //   FALSE
        //   POP
        //   POP
        // as a quick test
        0x00, 0x01, 0x02, 0x02,

        // Section header
        // --------------
        // Fourth section is data
        '.', 'd', 'a', 't', 'a',
        
        // Data section
        // ------------
        // End of the file is just assorted data. This is our 'constant pool,' 
        // where lookups from things such as .strings will take us to get data.
        // 
        // String 'test'
        't', 'e', 's', 't',
        // String '()V'
        '(', ')', 'V',
        'I',

        // That's it. We're done.
    };

    fwrite(data, sizeof(data) / sizeof(char), 1, file);
    fclose(file);
    return 0;
}
