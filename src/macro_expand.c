#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macro_expand.h"
#include "macros_linked_list.h"

void validate_macro_name(char *macro_name) {
    // TODO : validate macro name
}

void main_macro_expand(char *filename) {
    printf("Opening file %s\n", filename);
    char line[80]; 
    char* first_word;
    bool is_macro = false;

    FILE* file = fopen(filename, "r");

    // Check if the file was opened successfully.
    if (file != NULL) {
        // Read each line from the file and store it in the
        while (fgets(line, sizeof(line), file)) {
            first_word = strtok(line, " ");
            printf("%s\n", first_word);
            if (strcmp(first_word, "mcro") == 0) {
                is_macro = true;
                validate_macro_name(first_word);
            } else {
                if (is_in_list(macro_table, first_word) != NULL) {
                    printf("Macro %s found in list\n", first_word);
            }
        }
        fclose(file);
    }
    else {
        // Print an error message to the standard error
        fprintf(stderr, "Unable to open file!\n");
    }
    // TODO : create .am file
    // if1 first in line is in MACRO_TABLE then paste the macro value
    // else1 
        // if2 first in line is "mcro"
        // validate name of macro
        // add macro_name to MACRO_TABLE
        // Read next line
        // else2
}
}