#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grammar.h"
#include "globals.h"

op_code OPCODES[] = {
    {"mov",  2},
    {"cmp",  2},
    {"add",  2},
    {"sub",  2},
    {"not",  1},
    {"clr",  1},
    {"lea",  2},
    {"inc",  1},
    {"dec",  1},
    {"jmp",  1},
    {"bne",  1},
    {"red",  1},
    {"prn",  1},
    {"jsr",  1},
    {"rts",  0},
    {"stop", 0}
};

char *REGS[] = {"@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7"};

char *INSTUCTIONS[] = {".data", ".string", ".extern", ".entry"};

int is_instruction(char *str) {
    int i;

    if (str == NULL) {
        return 0;
    }

    for (i = 0; i < INSTRUCTIONS_AMOUNT; i++) {
        if (strcmp(str, INSTUCTIONS[i]) == 0)
            return 1; 
    }
    return 0;
}


int opcode_num(char *str) {
    int i;

    if (str == NULL) {
        return -1;
    }

    for (i = 0; i < OPCODES_AMOUNT; i++) {
        if (strcmp(str, OPCODES[i].opcode) == 0)
            return i; 
    }
    return -1; 
}


int reg_num(char *str) {
    int i;

    if (str == NULL) {
        return -1;
    }

    for (i = 0; i < REGS_AMOUNT; i++) {
        if (strcmp(str, REGS[i]) == 0)
            return i; 
    }
    return -1; 
}

