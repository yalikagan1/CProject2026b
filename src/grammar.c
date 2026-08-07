#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grammar.h"
#include "globals.h"

static const Operation OPERATIONS[OPERATIONS_AMOUNT] = {
    {"add",   0,  1, INST_TYPE_R, OP_FORMAT_RRR,   3},
    {"sub",   0,  2, INST_TYPE_R, OP_FORMAT_RRR,   3},
    {"and",   0,  3, INST_TYPE_R, OP_FORMAT_RRR,   3},
    {"or",    0,  4, INST_TYPE_R, OP_FORMAT_RRR,   3},
    {"nor",   0,  5, INST_TYPE_R, OP_FORMAT_RRR,   3},
    {"move",  1,  1, INST_TYPE_R, OP_FORMAT_RR,    2},
    {"mvhi",  1,  2, INST_TYPE_R, OP_FORMAT_RR,    2},
    {"mvlo",  1,  3, INST_TYPE_R, OP_FORMAT_RR,    2},
    {"addi", 10, -1, INST_TYPE_I, OP_FORMAT_RIR,   3},
    {"subi", 11, -1, INST_TYPE_I, OP_FORMAT_RIR,   3},
    {"andi", 12, -1, INST_TYPE_I, OP_FORMAT_RIR,   3},
    {"ori",  13, -1, INST_TYPE_I, OP_FORMAT_RIR,   3},
    {"nori", 14, -1, INST_TYPE_I, OP_FORMAT_RIR,   3},
    {"bne",  15, -1, INST_TYPE_I, OP_FORMAT_RRL,   3},
    {"beq",  16, -1, INST_TYPE_I, OP_FORMAT_RRL,   3},
    {"blt",  17, -1, INST_TYPE_I, OP_FORMAT_RRL,   3},
    {"bgt",  18, -1, INST_TYPE_I, OP_FORMAT_RRL,   3},
    {"lb",   19, -1, INST_TYPE_I, OP_FORMAT_RIR,   3},
    {"sb",   20, -1, INST_TYPE_I, OP_FORMAT_RIR,   3},
    {"lw",   21, -1, INST_TYPE_I, OP_FORMAT_RIR,   3},
    {"sw",   22, -1, INST_TYPE_I, OP_FORMAT_RIR,   3},
    {"lh",   23, -1, INST_TYPE_I, OP_FORMAT_RIR,   3},
    {"sh",   24, -1, INST_TYPE_I, OP_FORMAT_RIR,   3},
    {"jmp",  30, -1, INST_TYPE_J, OP_FORMAT_JUMP,  1},
    {"la",   31, -1, INST_TYPE_J, OP_FORMAT_LABEL, 1},
    {"call", 32, -1, INST_TYPE_J, OP_FORMAT_LABEL, 1},
    {"hlt",  63, -1, INST_TYPE_J, OP_FORMAT_NONE,  0}
};

/* Every legal register name, so that the index is the register number */
static char *REGISTERS[MAX_REGISTER + 1] = {
    "$0",  "$1",  "$2",  "$3",  "$4",  "$5",  "$6",  "$7",
    "$8",  "$9",  "$10", "$11", "$12", "$13", "$14", "$15",
    "$16", "$17", "$18", "$19", "$20", "$21", "$22", "$23",
    "$24", "$25", "$26", "$27", "$28", "$29", "$30", "$31"
};

/* The names are kept without the leading dot, because a label may not be
   called asciz either */
static const struct {
    char *name;
    DirectiveType type;
} DIRECTIVES[DIRECTIVES_AMOUNT] = {
    {"db",     DIRECTIVE_DB},
    {"dh",     DIRECTIVE_DH},
    {"dw",     DIRECTIVE_DW},
    {"asciz",  DIRECTIVE_ASCIZ},
    {"entry",  DIRECTIVE_ENTRY},
    {"extern", DIRECTIVE_EXTERN}
};

const Operation *find_operation(char *name) {
    int i;

    if (name == NULL) {
        return NULL;
    }

    for (i = 0; i < OPERATIONS_AMOUNT; i++) {
        if (strcmp(name, OPERATIONS[i].name) == 0)
            return &OPERATIONS[i];
    }
    return NULL;
}


DirectiveType find_directive(char *name) {
    int i;

    if (name == NULL) {
        return DIRECTIVE_NONE;
    }

    /* The name is written with a dot in the source but the table holds the
       bare name, so both forms are accepted */
    if (*name == '.') {
        name++;
    }

    for (i = 0; i < DIRECTIVES_AMOUNT; i++) {
        if (strcmp(name, DIRECTIVES[i].name) == 0)
            return DIRECTIVES[i].type;
    }
    return DIRECTIVE_NONE;
}


int parse_register(char *str) {
    int i;

    if (str == NULL) {
        return -1;
    }

    for (i = MIN_REGISTER; i <= MAX_REGISTER; i++) {
        if (strcmp(str, REGISTERS[i]) == 0)
            return i;
    }
    return -1;
}


int is_reserved_word(char *name) {
    return find_operation(name) != NULL || find_directive(name) != DIRECTIVE_NONE;
}

