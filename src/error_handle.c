#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error_handle.h"

static Error errors[] = {
    {ERROR_TOO_LONG_LINE,  "Line length is too long"},
    {ERROR_EXTRA_CHARS_AFTER_MACRO,  "Extra characters after macro declaration"},
    {ERROR_INVALID_MACRO_NAME,  "Invalid macro name"},
    {ERROR_OPEN_FILE,  "Can't open file"}
};

/* Returns text of this error code. */
static char *error_text(int e) {
    int i;

    for (i = 0; i < sizeof(errors) / sizeof(errors[0]); i++) {
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
