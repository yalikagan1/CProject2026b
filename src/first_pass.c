/* first_pass.c - The first pass over the .am file: builds the symbol table,
   encodes the data image, and gives every instruction its address. */
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "error_handle.h"
#include "helpers.h"
#include "line_parser.h"
#include "first_pass.h"
#include "symbols_table_handler.h"
#include "data_image_handler.h"
#include "code_image_handler.h"


int first_pass(char *am_filename, Symbol **symbols, CodeImage *code, DataImage *data, int *icf, int *dcf) {
    int IC = INITIAL_IC;
    int DC = INITIAL_DC;
    int no_errors = 1;

    char line[MAX_LINE_LENGTH + 2];
    char *trimmed_line;
    int line_number = 0;
    ParsedLine parsed_line;
    int err;

    FILE * am_file = fopen(am_filename, "r");
    if (am_file == NULL) {
        print_error(ERROR_OPEN_FILE, am_filename, 0);
        return 0;
    }

    data->count = INITIAL_DC;

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

        err = parse_line(trimmed_line, &parsed_line);
        if (err) {
            print_error(err, am_filename, line_number);
            no_errors = 0;
            continue;
        }
        err = create_symbol(parsed_line, symbols, &IC, &DC);
        if (err) {
            print_error(err, am_filename, line_number);
            no_errors = 0;
            continue;
        }

        if (parsed_line.type == DIRECTIVE_DB || parsed_line.type == DIRECTIVE_DH ||
            parsed_line.type == DIRECTIVE_DW || parsed_line.type == DIRECTIVE_ASCIZ) {
            err = encode_data_directive(parsed_line.type, parsed_line.operands, data);
            if (err) {
                print_error(err, am_filename, line_number);
                no_errors = 0;
                continue;
            }

            DC = data->count;
        }
        else if (parsed_line.type == DIRECTIVE_NONE) {
            err = add_instruction(parsed_line, code);
            if (err) {
                print_error(err, am_filename, line_number);
                no_errors = 0;
                continue;
            }
            IC += 4;
        }
    }

    fclose(am_file);

    *icf = IC;
    *dcf = DC;

    update_all_data_symbols(*symbols, IC);

    return no_errors;
}
