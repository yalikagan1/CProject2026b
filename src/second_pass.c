#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "second_pass.h"
#include "error_handle.h"
#include "helpers.h"

int second_pass(char *am_filename, Symbol **symbols, CodeImage *code,
    DataImage *data, int *icf, int *dcf) {



        write_entries(am_filename, symbols);
        write_externals(am_filename, symbols, code);
    }



void write_entries(char *am_filename, Symbol **symbols) {
    char* output_file = NULL;
    Symbol *head = *symbols;
    FILE *file_write;

    output_file = add_file_extention(am_filename, ".ent");
    file_write = fopen(output_file, "w");

    if(file_write == NULL) {
        print_error(ERROR_OPEN_FILE, am_filename, 0);
        return;
    }

    while (head != NULL) {
        if(head->is_entry) {
            fprintf(file_write, "%s ", head->name);
            fprintf(file_write, "%04d\n", head->value);
        }
        head = head->next;
    }

    fclose(file_write);
}

void write_externals(char *am_filename, Symbol **symbols, CodeImage *code) {
    char* output_file = NULL;
    Symbol *head = *symbols;
    FILE *file_write;
    Operation op;

    output_file = add_file_extention(am_filename, ".ext");
    file_write = fopen(output_file, "w");

    if(file_write == NULL) {
        print_error(ERROR_OPEN_FILE, am_filename, 0);
        return;
    }

    while(code != NULL) {
        op = code->current;
        if(is_operand_external(op.arg1, *symbols)) {
            fprintf(file_write, "%s ", op.arg1);
            fprintf(file_write, "%04d\n", code->current_ic);
        }

        if(is_operand_external(op.arg2, *symbols)) {
            fprintf(file_write, "%s ", op.arg2);
            fprintf(file_write, "%04d\n", code->current_ic);
        }

        if(is_operand_external(op.arg3, *symbols)) {
            fprintf(file_write, "%s ", op.arg3);
            fprintf(file_write, "%04d\n", code->current_ic);
        }
        code = code->next;
    }

    fclose(file_write);
}
