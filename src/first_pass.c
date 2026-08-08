/* first_pass.c - The first pass over the .am file: builds the symbol table,
   encodes the data image, and gives every instruction its address. */
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "error_handle.h"
#include "helpers.h"
#include "first_pass.h"


int first_pass(char *am_filename, Symbol **symbols, CodeImage *code, DataImage *data, int *icf, int *dcf) {
    int IC = INITIAL_IC;
    int DC = INITIAL_DC;
    int no_errors = 1;

    char line[MAX_LINE_LENGTH + 2];
    char *trimmed_line;
    int line_number = 0;

    FILE * am_file = fopen(am_filename, "r");
    if (am_file == NULL) {
        print_error(ERROR_OPEN_FILE, am_filename, 0);
        return 0;
    }

    while (fgets(line, sizeof(line), am_file) != NULL) {
        line_number++;
        
        if (!check_if_line_is_ended(line)) {
            if (remove_rest_of_line(am_file)) {
                print_error(ERROR_TOO_LONG_LINE, am_filename, line_number);
                no_errors = 0;
                continue;
            }
        }

        trimmed_line = trim_whitespaces(line);

        /* comment line or empty line that not need to assemble*/
        if (trimmed_line[0] == '\0' || trimmed_line[0] == ';'){
            continue;
        }
    }

    fclose(am_file);

    *icf = IC;
    *dcf = DC;

    return no_errors;
}
