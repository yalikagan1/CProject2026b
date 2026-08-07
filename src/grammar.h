/* grammar.h - Constants and data structures for the grammar of the assembly language. */
#ifndef GRAMMAR_H
#define GRAMMAR_H

#define OPERATIONS_AMOUNT 27

#define DIRECTIVES_AMOUNT 6

#define MIN_REGISTER 0

#define MAX_REGISTER 31

typedef enum {
    DIRECTIVE_NONE,
    DIRECTIVE_DB,
    DIRECTIVE_DH,
    DIRECTIVE_DW,
    DIRECTIVE_ASCIZ,
    DIRECTIVE_ENTRY,
    DIRECTIVE_EXTERN
} DirectiveType;

typedef enum {
    OP_FORMAT_RRR,
    OP_FORMAT_RR,
    OP_FORMAT_RIR,
    OP_FORMAT_RRL,
    OP_FORMAT_JUMP,
    OP_FORMAT_LABEL,
    OP_FORMAT_NONE
} OperandFormat;

typedef enum {
    INST_TYPE_R,
    INST_TYPE_I,
    INST_TYPE_J
} InstructionType;

typedef struct {
    char *name;    /* The name corresponding to the operation */
    int opcode; /* The number of the opcode */
    int funct; /* The funct code of the operation, -1 when the operation has no funct field */
    InstructionType type; /* The type of the operation */
    OperandFormat format; /* The format of the operands */
    int arg_num;     /* The number of arguments for the operation */
} Operation;

/* Returns the operation with this name, or NULL if there is no such operation */
const Operation *find_operation(char *name);

/* Returns the directive with this name, or DIRECTIVE_NONE if there is no such directive */
DirectiveType find_directive(char *name);

/* Returns the register number of a string like $5, or -1 if it is not a register */
int parse_register(char *str);

/* Returns 1 if the name is an operation name or a directive name */
int is_reserved_word(char *name);

#endif
