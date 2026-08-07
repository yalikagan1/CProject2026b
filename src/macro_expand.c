#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "macro_expand.h"
#include "macros_linked_list.h"
#include "error_handle.h"
#include "helpers.h"
#include "grammar.h"

int validate_macro_name(char *macro_name, char *filename, int line_counter) {
    macro_name[strcspn(macro_name, "\n")] = '\0';
    if(is_reserved_word(macro_name)) {
        print_error(ERROR_CODE_3, filename, line_counter);
        return 1;
    }
    return 0;
}

void main_macro_expand(char *filepath) {
 
    char line[80], temp_line[80]; 
    char* first_word, *current_mcro_txt = NULL,  *mcr_name, *dynamic_mcr_name, *filename = NULL, *output_file = NULL;
    bool is_macro = false;
    int line_counter = 0, found_error = 0;
    node *head = NULL, *temp_node;
    FILE *file, *file_write;
    filename = add_file_extention(filepath, ".as");
    output_file = add_file_extention(filepath, ".am");

    file = fopen(filename, "r");
    file_write = fopen(output_file, "w");

    /* Check if the file was opened successfully. */
    if (file != NULL ) {
        /* Read each line from the file and store it in the */
        while (fgets(line, sizeof(line), file)) {
            line_counter++;
            /* validate line length */
            found_error += check_line_length(line, filename, line_counter);

            /* remove tabs and extra whitespaces from line */
            remove_tabs(line);
            remove_extra_whitespaces(line);

            strcpy(temp_line, line);

            /* get first word in line */
            first_word = strtok(temp_line, " ");
            if(is_macro) {
                if(strcmp(first_word, "mcroend\n") == 0) {
                    is_macro = false;
                    temp_node->macro_content = current_mcro_txt;
                    current_mcro_txt = NULL;
                    if(head) {
                        add_node(head, temp_node);
                    } else {
                        head = temp_node;
                    }
                } else {
                    current_mcro_txt = realloc(current_mcro_txt, strlen(current_mcro_txt) + strlen(line));
                    strcat(current_mcro_txt, line);
                }
            } else {
                if(strcmp(first_word, "mcro") == 0) {
                    is_macro = true;

                    mcr_name = strtok(NULL, "");
                    found_error += validate_macro_name(mcr_name, filename, line_counter);
                    dynamic_mcr_name = malloc(strlen(mcr_name));
                    strcpy(dynamic_mcr_name, mcr_name);

                    temp_node = create_node(dynamic_mcr_name);
                    current_mcro_txt = malloc(0);

                    if(strtok(NULL, "")) {
                        print_error(ERROR_CODE_2, filename, line_counter);
                        found_error = 1;
                    }
                } else {
                    first_word[strcspn(first_word, "\n")] = '\0';
                    current_mcro_txt = is_in_list(head, first_word);
                    if(current_mcro_txt) { // Found macro use
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
        free(filename);
        free(output_file);
    } else {
        print_error(ERROR_CODE_4, filename, 0);
    }
    if(found_error) {
        remove(output_file);
    }
}
