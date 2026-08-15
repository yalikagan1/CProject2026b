#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "macro_expand.h"
#include "macros_linked_list.h"
#include "error_handle.h"
#include "helpers.h"
#include "grammar.h"

/* A macro name opens with a letter and carries letters, digits and underscores.
   The underscore is the one way it is freer than a label. It may not be the name
   of an operation or of a directive, or a line that uses it would read like a
   line of code */
static int is_legal_macro_name(char *name) {
    int i;

    if(!isalpha((unsigned char)name[0])) {
        return 0;
    }

    for(i = 1; name[i] != '\0'; i++) {
        if(!isalnum((unsigned char)name[i]) && name[i] != '_') {
            return 0;
        }
    }

    return !is_reserved_word(name);
}

int validate_macro_name(char *macro_name, char *filename, int line_counter) {
    if(macro_name == NULL) {
        print_error(ERROR_MISSING_MACRO_NAME, filename, line_counter);
        return 1;
    }

    macro_name[strcspn(macro_name, "\n")] = '\0';

    if(strlen(macro_name) > MAX_LABEL_LENGTH || !is_legal_macro_name(macro_name)) {
        print_error(ERROR_INVALID_MACRO_NAME, filename, line_counter);
        return 1;
    }

    return 0;
}

int main_macro_expand(char *filepath) {
 
    /* a legal line is 80 characters, and there is also the newline and the \0 */
    char line[MAX_LINE_LENGTH + 2], temp_line[MAX_LINE_LENGTH + 2];
    char* first_word, *current_mcro_txt = NULL,  *mcr_name, *dynamic_mcr_name, *filename = NULL, *output_file = NULL;
    int is_macro = 0;
    int line_counter = 0, found_error = 0;
    node *head = NULL, *temp_node;
    FILE *file, *file_write;
    filename = add_file_extention(filepath, ".as");
    output_file = add_file_extention(filepath, ".am");

    file = fopen(filename, "r");
    file_write = fopen(output_file, "w");

    /* Check if the file was opened successfully. */
    if (file != NULL && file_write != NULL) {
        /* Read each line from the file and store it in the */
        while (fgets(line, sizeof(line), file)) {
            line_counter++;
            /* validate line length */
            found_error += check_line_length(line, file, filename, line_counter);

            /* remove tabs and extra whitespaces from line */
            remove_tabs(line);
            remove_extra_whitespaces(line);

            strcpy(temp_line, line);

            /* get first word in line */
            first_word = strtok(temp_line, " ");

            /* a line that holds nothing but separators has no first word */
            if(first_word == NULL) {
                fprintf(file_write, "%s", line);
                continue;
            }

            /* the newline is in the way of every comparison below */
            first_word[strcspn(first_word, "\n")] = '\0';

            if(is_macro) {
                if(strcmp(first_word, "mcroend") == 0) {
                    /* nothing may share the line that closes a macro */
                    if(strtok(NULL, "") != NULL) {
                        print_error(ERROR_EXTRA_CHARS_AFTER_MACRO, filename, line_counter);
                        found_error = 1;
                    }

                    is_macro = 0;
                    temp_node->macro_content = current_mcro_txt;
                    current_mcro_txt = NULL;
                    if(head) {
                        add_node(head, temp_node);
                    } else {
                        head = temp_node;
                    }
                } else {
                    current_mcro_txt = realloc(current_mcro_txt, strlen(current_mcro_txt) + strlen(line) + 1);
                    strcat(current_mcro_txt, line);
                }
            } else {
                if(strcmp(first_word, "mcro") == 0) {
                    is_macro = 1;

                    mcr_name = strtok(NULL, " ");
                    found_error += validate_macro_name(mcr_name, filename, line_counter);

                    if(mcr_name == NULL) {
                        mcr_name = "";
                    }

                    dynamic_mcr_name = malloc(strlen(mcr_name) + 1);
                    strcpy(dynamic_mcr_name, mcr_name);

                    temp_node = create_node(dynamic_mcr_name);

                    current_mcro_txt = malloc(1);
                    current_mcro_txt[0] = '\0';

                    if(strtok(NULL, "")) {
                        print_error(ERROR_EXTRA_CHARS_AFTER_MACRO, filename, line_counter);
                        found_error = 1;
                    }
                } else {
                    current_mcro_txt = (*first_word == '\0') ? NULL : is_in_list(head, first_word);
                    if(current_mcro_txt) { /* Found macro used */
                        fprintf(file_write, "%s", current_mcro_txt);
                    } else {              
                        fprintf(file_write, "%s", line);
                    }
                }
            }
        }
        fclose(file);
        fclose(file_write);
        free_list(head);
    } else {
        found_error = 1;

                if(file == NULL) {
            print_error(ERROR_OPEN_FILE, filename, 0); /* input file */
        } else {
            print_error(ERROR_OPEN_FILE, output_file, 0); /* output file */
            fclose(file);
        }

        if(file_write != NULL) {
            fclose(file_write);
        }
    }

    free(filename);
    if(found_error) {
        remove(output_file);
    }
    free(output_file);

    return found_error;
}
