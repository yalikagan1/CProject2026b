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

    int encoded_instructions[] = {
        6637632,
        891486203,
        2080374784,
        2013266036,
        109060160,
        1216479260,
        2080374945,
        1477050372,
        1072300004,
        -2147483647 - 1, // -2147483648 (INT32_MIN)
        2046820356,
        2080374784,
        -67108864
    };
    
    char str[12];
    for(int i = 0; i < 13; i++) {
        int_to_hex(encoded_instructions[i], str);
        printf("%s\n", str);
    }
    return 0;
}