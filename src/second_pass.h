#include "globals.h"
#include "grammar.h"

int second_pass(char *am_filename, Symbol **symbols, CodeImage *code,
    DataImage *data, int *icf, int *dcf);

void write_entries(char *am_filename, Symbol **symbols);

void write_externals(char *am_filename, Symbol **symbols, CodeImage *code);