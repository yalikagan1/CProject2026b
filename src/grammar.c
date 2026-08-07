#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grammar.h"
#include "globals.h"

op_code OPCODES[] = {
    {"add",  3},
    {"sub",  3},
    {"and",  3},
    {"or",  3},
    {"nor",  3},
    {"move",  2},
    {"mvhi",  2},
    {"mvlo",  2},
    {"addi",  3},
    {"subi",  3},
    {"andi",  3},
    {"ori",  3},
    {"nori",  3},
    {"bne",  3},
    {"beq",  3},
    {"blt",  3},
    {"bgt",  3},
    {"lb",  3},
    {"sb",  3},
    {"lw",  3},
    {"sw",  3},
    {"lh",  3},
    {"sh",  3},
    {"jmp",  1},
    {"la",  1},
    {"call",  1},
    {"hlt", 0}
};

char *REGS[] = {"$0", "$1", "$2", "$3", "$4", "$5", "$6", "$7",
                "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15",
                "$16", "$17", "$18", "$19", "$20", "$21", "$22", "$23",
                "$24", "$25", "$26", "$27", "$28", "$29", "$30", "$31"};

char *INSTUCTIONS[] = {".dh", ".dw", ".db", ".asciz", ".extern", ".entry"};

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

