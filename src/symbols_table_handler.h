#ifndef SYMBOLS_TABLE_HANDLER_H
#define SYMBOLS_TABLE_HANDLER_H

#include "line_parser.h"
#include "grammar.h"

int create_symbol(ParsedLine parsed_line, Symbol **symbols, int *icf, int *dcf, int line_number);

void update_all_data_symbols(Symbol *symbols, int icf);

/* Returns the symbol with this name, or NULL if the table has no such symbol */
Symbol *find_symbol(char *name, Symbol **symbols);

/* Reports every symbol that got a .entry but was never defined in the file.
   Returns 1 when all the entries are fine, 0 when at least one is not */
int check_all_entries_defined(Symbol *symbols, char *filename);

/* Frees every symbol in the table and leaves the table empty */
void free_symbols_table(Symbol **symbols);

#endif
