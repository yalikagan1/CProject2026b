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
    {ERROR_MISSING_OPERATION,  "Line has no operation or directive"}
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
