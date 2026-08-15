#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>

/**
 * @brief remove tabs from string.
 *
 * This function remove tabs from string.
 * @param str string to remove tabs from.
 */
void remove_tabs(char *str);

/**
 * @brief remove extra whitespaces from string.
 *
 * This function remove extra whitespaces from string.
 * @param str string to remove extra whitespaces from.
 */
void remove_extra_whitespaces(char *str);

/**
 * @brief validate line length.
 *
 * This function validate line length according to assembler rules. A line that
 * is too long is thrown away up to its end, so the reading goes on from the
 * next line.
 * @param line string line text.
 * @param file the file the line was read from.
 * @param filename name of the input file.
 * @param line_counter number of line in the input file.
 * @return 0 if valid, 1 if not valid.
 */
int check_line_length(char *line, FILE *file, char *filename, int line_counter);

/**
 * @brief add extension to file.
 *
 * This function add extension to file..
 * @param filename string name of the input file.
 * @param filename string desired extension.
 * @return string filepath with desired extension.
 */
char* add_file_extention(char *filepath, char *ext);

char *trim_whitespaces(char *str);

/* Returns a pointer to the first character that is not a space */
char *skip_spaces(char *str);

/* Returns 1 if the line is ended with a newline */
int check_if_line_is_ended(char *line);

/* Reads and throws away everything up to the end of the current line.
   Returns 1 if there was anything left, which means the line really was cut,
   and 0 if the file ended right there and nothing was left */
int remove_rest_of_line(FILE *file);

#endif
