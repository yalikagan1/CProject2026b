#include <stdio.h>
#include "macro_expand.h"
#include "second_pass.h"

int main()
{
    //main_macro_expand("/Users/yali/2026b/projects/CProject2026b/test_files/test2");
    // int num;
    // Operation op;
    // op.opcode = 13;
    // op.arg1 = "$9";
    // op.arg2 = "-5";
    // op.arg3 = "$2";
    // op.type = INST_TYPE_I;
    // int current_ic = 100;
    // for(int i = 1; i < 4; i++) {
    //     op.funct = i;
    //     num = create_binary_code(op, NULL, current_ic);
    //     printf("*******i = %d - ", i);
    //     print_as_binary(num);
    // }

    // int encoded_instructions[] = {
    //     6637632,
    //     891486203,
    //     2080374784,
    //     2013266036,
    //     109060160,
    //     1216479260,
    //     2080374945,
    //     1477050372,
    //     1072300004,
    //     -2147483647 - 1, // -2147483648 (INT32_MIN)
    //     2046820356,
    //     2080374784,
    //     -67108864
    // };
    
    // char str[12];
    // int i;
    // for(i = 0; i < 13; i++) {
    //     int_to_hex(encoded_instructions[i], str);
    //     printf("%s\n", str);
    // }

    char data[] = {
        'a',        // Line 1:  01100001
        'B',        // Line 2:  01000010
        'c',        // Line 3:  01100011
        'd',        // Line 4:  01100100
        '\0',       // Line 5:  00000000
        '\x06',     // Line 6:  00000110 (Control char: ACK)
        '\xF7',     // Line 7:  11110111 (Extended ASCII / 247)
        
        // Line 8 (2 bytes -> Rightmost first: 10110000, then 01101001)
        '\xB0', 'i', 
        
        // Line 9 (4 bytes -> Rightmost first: 00001111, then 0s)
        '\x1F', '\0', '\0', '\0', 
        
        // Line 10 (4 bytes -> Rightmost first: 11110100, then 11111111s)
        '\xF4', '\xFF', '\xFF', '\xFF'
    };

    int length = 17, current_counter = 148, i;

    for(i = 0; i < length; i++) {
        if(i%4 == 0) {
            current_counter += 4;
            printf("%04d ", current_counter);
        }
        printf("%02X ", (unsigned char)data[i]);
        if(i%4 == 3)
            printf("\n");
    }

    return 0;
}
