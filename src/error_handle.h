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
    ERROR_OPEN_FILE = 4
} ERROR_CODES;

void print_error(int e, char *filename, int line);

void print_inner_error(int e);

#endif
