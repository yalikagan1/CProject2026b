#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error_handle.h"

Error errors[] = {
    {ERROR_CODE_1,  "Line length is too long"},
    {ERROR_CODE_2,  "Extra characters after macro declaration"},
    {ERROR_CODE_3,  "Invalid macro name"}
};

void print_inner_error(int e) {
    printf("#### ERROR #%d - %s\n", e, errors[e-1].txt);
}

void print_error(int e, char *filename, int line) {
    printf("#### ERROR #%d at file %s, line %d - %s\n", e, filename, line, errors[e-1].txt);
}
