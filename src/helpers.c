#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
        print_error(ERROR_TOO_LONG_LINE, filename, line_counter);
    return ret_val;
}

char *skip_spaces(char *str) {
    while (isspace((unsigned char)*str)) {
        str++;
    }

    return str;
}

char *trim_whitespaces(char *str) {
    char *end;

    /* skipping whitespeces and tabs*/
    while (*str == ' ' || *str == '\t') {
        str++;
    }

    /* \r is there when the file was edited on Windows */
    end = str + strlen(str);
    while (end > str && (end[-1] == ' ' || end[-1] == '\t' ||
                         end[-1] == '\n' || end[-1] == '\r')) {
        end--;
    }
    *end = '\0';

    return str;
}

int check_if_line_is_ended(char *line) {
    return strchr(line, '\n') != NULL;
}

int remove_rest_of_line(FILE *file) {
    int c = getc(file);

    /* If the file ended right there, nothing was left */
    if (c == EOF) {
        return 0;
    }

    /* Read and throw away everything up to the end of the current line */
    while (c != '\n' && c != EOF) {
        c = getc(file);
    }

    return 1;
}

char * add_file_extention(char *filepath, char *ext) {
    char *filename = malloc(strlen(filepath) + strlen(ext));
    strcpy(filename, filepath);
    strcat(filename, ext);
    return filename;
}
