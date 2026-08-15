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
    ERROR_DATA_IMAGE_FULL = 15,
    ERROR_MEMORY_ALLOCATION_FAILED = 16,
    ERROR_SYMBOL_ALREADY_EXISTS_BUT_INVALID = 17,
    ERROR_SYMBOL_ENTRY_AND_EXTERN = 18,
    ERROR_TOO_MANY_OPERANDS = 19,
    ERROR_TOO_FEW_OPERANDS = 20,
    ERROR_INVALID_OPERAND = 21,
    ERROR_INVALID_FORMAT = 22,
    ERROR_ENTRY_NOT_DEFINED = 23,
    ERROR_UNKNOWN_OPERATION = 24,
    ERROR_UNDEFINED_SYMBOL = 25
} ERROR_CODES;

/**
 * @brief prints error in input file.
 *
 * This function prints error in input file.
 * @param e number of error - according to ERROR_CODES.
 * @param filename name of the input file.
 * @param line number of line in the input file.
 */
void print_error(int e, char *filename, int line);

/**
 * @brief prints error in inner process.
 *
 * This function prints error in inner process.
 * @param e number of error - according to ERROR_CODES.
 */
void print_inner_error(int e);

#endif
