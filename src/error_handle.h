#ifndef ERROR_HANDLE_H
#define ERROR_HANDLE_H

typedef struct Error {
    int id;
    char *txt;
} Error;

typedef enum ERROR_CODES {
    ERROR_TOO_LONG_LINE = 1,
    ERROR_EXTRA_CHARS_AFTER_MACRO = 2,
    ERROR_INVALID_MACRO_NAME = 3,
    ERROR_OPEN_FILE = 4,
    ERROR_LABEL_TOO_LONG = 5,
    ERROR_INVALID_LABEL_NAME = 6,
    ERROR_LABEL_IS_RESERVED_WORD = 7,
    ERROR_MISSING_OPERATION = 8,
    ERROR_UNKNOWN_DIRECTIVE = 9,
    ERROR_MISSING_OPERANDS = 10,
    ERROR_INVALID_NUMBER = 11,
    ERROR_NUMBER_OUT_OF_RANGE = 12,
    ERROR_INVALID_COMMAS = 13,
    ERROR_INVALID_STRING = 14,
    ERROR_DATA_IMAGE_FULL = 15
} ERROR_CODES;

void print_error(int e, char *filename, int line);

void print_inner_error(int e);

#endif
