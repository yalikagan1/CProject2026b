#include <stdio.h>
#include <stdlib.h>
#include "grammar.h"
#include "helpers.h"
#include "macro_expand.h"
#include "first_pass.h"
#include "second_pass.h"
#include "code_image_handler.h"
#include "symbols_table_handler.h"


static void assemble_file(char *filepath) {
    Symbol *symbols = NULL;
    CodeImage *code = NULL;
    DataImage data;
    char *am_filename;
    FILE *am_file;
    int icf = 0;
    int dcf = 0;

    if (main_macro_expand(filepath)) {
        return;
    }

    am_filename = add_file_extention(filepath, ".am");

    am_file = fopen(am_filename, "r");
    if (am_file == NULL) {
        free(am_filename);
        return;
    }
    fclose(am_file);

    if (first_pass(am_filename, &symbols, &code, &data, &icf, &dcf)) {
        second_pass(filepath, symbols, code, &data, &icf, &dcf);
    }

    free_code_image(&code);
    free_symbols_table(&symbols);
    free(am_filename);
}

int main(int argc, char *argv[]) {
    int i;

    if (argc < 2) {
        printf("Usage: assembler file1 file2 ...\n");
        printf("The files are given without the .as extension\n");
        return 1;
    }

    for (i = 1; i < argc; i++) {
        assemble_file(argv[i]);
    }

    return 0;
}
