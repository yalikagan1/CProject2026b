#ifndef SYMBOLS_TABLE_HANDLER_H
#define SYMBOLS_TABLE_HANDLER_H

#include "line_parser.h"
#include "grammar.h"

int create_symbol(ParsedLine parsed_line, Symbol **symbols, int *icf, int *dcf);

#endif
