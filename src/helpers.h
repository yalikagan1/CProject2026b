#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>

void remove_tabs(char *str);

void remove_extra_whitespaces(char *str);

int check_line_length(char *line, char *filename, int line_counter);

char* add_file_extention(char *filepath, char *ext);

char *trim_whitespaces(char *str);

/* Returns 1 if the line is ended with a newline */
int check_if_line_is_ended(char *line);

/* Reads and throws away everything up to the end of the current line.
   Returns 1 if there was anything left, which means the line really was cut,
   and 0 if the file ended right there and nothing was left */
int remove_rest_of_line(FILE *file);

#endif
