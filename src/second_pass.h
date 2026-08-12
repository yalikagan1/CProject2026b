#include "globals.h"
#include "grammar.h"

int second_pass(char *am_filename, Symbol *symbols, CodeImage *code,
    DataImage *data, int *icf, int *dcf);

void write_entries(char *am_filename, Symbol *symbols);

void write_externals(char *am_filename, Symbol *symbols, CodeImage *code);

int create_binary_code(Operation op, Symbol *s, int current_ic);

int create_r_code(Operation op);

int create_i_code(Operation op, int current_ic);

int create_j_code(Operation op, Symbol *s);

int get_symbol_value(char *arg, Symbol *s);

void print_as_binary(int num);
