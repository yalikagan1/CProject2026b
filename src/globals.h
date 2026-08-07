/* globals.h - Constants and data structures shared by all stages of the assembler. */
#ifndef GLOBALS_H
#define GLOBALS_H

#define MAX_LABEL_LENGTH 31

#define MAX_LINE_LENGTH 80

#define MAX_OPERANDS 3

#define IC_START 100

#define MAX_CODE_WORDS 2048

#define MAX_DATA_BYTES 8192

#define OPCODES_AMOUNT 16

#define REGS_AMOUNT 32

#define INSTRUCTIONS_AMOUNT 6

typedef struct Symbol {
    char name[MAX_LABEL_LENGTH + 1];
    int value;
    int is_code;
    int is_data;
    int is_external;
    int is_entry;
    struct Symbol *next;
} Symbol;


typedef struct {
    unsigned long words[MAX_CODE_WORDS];
    int count;
} CodeImage;

typedef struct {
    char bytes[MAX_DATA_BYTES];
    int count;
} DataImage;

typedef struct ExternalRef {
    char name[MAX_LABEL_LENGTH + 1];
    int address;
    struct ExternalRef *next;
} ExternalRef;

#endif
