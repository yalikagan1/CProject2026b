#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helpers.h"
#include "error_handle.h"

void remove_tabs(char *str)
{
    char *src = str;
    char *dst = str;

    while (*src) {
        if (*src != '\t') {
            *dst++ = *src;
        }
        src++;
    }

    *dst = '\0';
}

void remove_extra_whitespaces(char *str) {
    int i, j = 0;

    for (i = 0; str[i]; i++) {
        if (str[i] != ' ' || (j > 0 && str[j - 1] != ' '))
            str[j++] = str[i];
    }

    str[j] = '\0';
}

int check_line_length(char *line, char *filename, int line_counter) {
    int ret_val = strchr(line, '\n') == NULL;
    if(ret_val)
        print_error(ERROR_CODE_1, filename, line_counter);
    return ret_val;
}
