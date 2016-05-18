#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    FILE* file = fopen("test.adri", "wb");
    char data[] = {
        // File header 
        // -----------
        // 0xDEADBEEFBABECAFE
        // Yes, I was trying to cram 4 'words' into 16 bytes.
        0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xBE, 0xCA, 0xFE,

        // Section header
        // --------------
        // First section is string lookup table
        '.', 's', 't', 'r', 'i', 'n', 'g', 's',
        // String number, string start, length. Start and length are in bytes.
        // Start is the 'address' of the string's first character in the file.
        // Length is just how many bytes the string is.
        // 
        // Our test string is 'test', hence size=0x04
        //
        // => String 0x00
        // |  => Starts at 0x2B
        // |  |     => Length of 0x04
        0x00, 0x2B, 0x04, 
        0x01, 0x2E, 0x03,
        
        // Section header
        // --------------
        // Second section is functions
        '.', 'f', 'u', 'n', 'c', 't', 'i', 'o', 'n', 's',

        // Function header 
        // ---------------
        // 0x03 is the function declaration opcode. The next two bytes are 
        // string identifiers. The first string is the function name, and the
        // second string is the function signature. 
        // The next four bytes are an integer representing the length of the
        // function, in bytes.
        // 
        // => Function opcode
        // |  => Function name: string 0x01
        // |  |     => Function signature: string 0x02
        // |  |     |     => Length of this function: 0x00000000
        0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04,
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

        // Data section
        // ------------
        // End of the file is just assorted data. There's no section header
        // here because there's no need; once we hit the end of .functions, we
        // know that there's going to be nothing but data left over. This is 
        // our 'constant pool,' where lookups from things such as .strings will
        // take us to get data
        // 
        // String 'test'
        't', 'e', 's', 't',
        // String '()V'
        '(', ')', 'V',

        // That's it. We're done.
    };

    fwrite(data, 50, 1, file);
    return 0;
}
