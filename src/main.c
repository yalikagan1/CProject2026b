#include <stdio.h>
#include "macro_expand.h"
#include "second_pass.h"

int main()
{
    //main_macro_expand("/Users/yali/2026b/projects/CProject2026b/test_files/test2");
    int num;
    Operation op;
    op.opcode = 13;
    op.arg1 = "$9";
    op.arg2 = "-5";
    op.arg3 = "$2";
    op.type = INST_TYPE_I;
    int current_ic = 100;
    for(int i = 1; i < 4; i++) {
        op.funct = i;
        num = create_binary_code(op, NULL, current_ic);
        printf("*******i = %d - ", i);
        print_as_binary(num);
    }
    return 0;
}