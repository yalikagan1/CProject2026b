#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error_handle.h"

static Error errors[] = {
    {ERROR_TOO_LONG_LINE,  "Line length is too long"},
    {ERROR_EXTRA_CHARS_AFTER_MACRO,  "Extra characters after macro declaration"},
    {ERROR_INVALID_MACRO_NAME,  "Invalid macro name"},
    {ERROR_OPEN_FILE,  "Can't open file"},
    {ERROR_LABEL_TOO_LONG,  "Label is longer than 31 characters"},
    {ERROR_INVALID_LABEL_NAME,  "Label must start with a letter and hold only letters and digits"},
    {ERROR_LABEL_IS_RESERVED_WORD,  "Label can't be a reserved word"},
    {ERROR_MISSING_OPERATION,  "Line has no operation or directive"},
    {ERROR_UNKNOWN_DIRECTIVE,  "Unknown directive"},
    {ERROR_MISSING_OPERANDS,  "Directive has no operands"},
    {ERROR_INVALID_NUMBER,  "Operand is not a valid number"},
    {ERROR_NUMBER_OUT_OF_RANGE,  "Number does not fit in the size of the directive"},
    {ERROR_INVALID_COMMAS,  "Commas between the operands are not right"},
    {ERROR_INVALID_STRING,  "String must be printable text wrapped in double quotes"},
    {ERROR_DATA_IMAGE_FULL,  "Too much data for the memory of the machine"},
    {ERROR_MEMORY_ALLOCATION_FAILED,  "Memory allocation failed"},
    {ERROR_SYMBOL_ALREADY_EXISTS_BUT_INVALID,  "Symbol is already defined in this file"},
    {ERROR_SYMBOL_ENTRY_AND_EXTERN,  "Symbol can't be both an entry and an external"}
};

/* Returns text of this error code. */
static char *error_text(int e) {
    int i;
    int errors_amount = sizeof(errors) / sizeof(errors[0]);

    for (i = 0; i < errors_amount; i++) {
        if (errors[i].id == e) {
            return errors[i].txt;
        }
    }

    return "Unknown error";
}

void print_inner_error(int e) {
    printf("#### ERROR #%d - %s\n", e, error_text(e));
}

void print_error(int e, char *filename, int line) {
    printf("#### ERROR #%d at file %s, line %d - %s\n", e, filename, line, error_text(e));
}
