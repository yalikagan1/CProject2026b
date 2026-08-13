#include <stdlib.h>
#include <string.h>
#include "symbols_table_handler.h"
#include "line_parser.h"
#include "grammar.h"
#include "error_handle.h"

int check_symbol_validity(Symbol *symbol, ParsedLine parsed_line) {
    if (parsed_line.type == DIRECTIVE_ENTRY) {
        if (symbol->is_external) { /* if the symbol is external, it cannot be an entry */
            return ERROR_SYMBOL_ENTRY_AND_EXTERN;
        }
        return 0;
    }
    if (parsed_line.type == DIRECTIVE_EXTERN) {
        if (symbol->is_entry) { /* if the symbol is an entry, it cannot be an extern */
            return ERROR_SYMBOL_ENTRY_AND_EXTERN;
        }
        if (symbol->is_code || symbol->is_data) { /* if the symbol is code or data, it cannot be an extern */
            return ERROR_SYMBOL_ALREADY_EXISTS_BUT_INVALID;
        }
        return 0;
    }

    if (symbol->is_code || symbol->is_data || symbol->is_external) { /*duplicate symbol*/
        return ERROR_SYMBOL_ALREADY_EXISTS_BUT_INVALID;
    }

    return 0;
}

int update_symbol(Symbol *symbol, ParsedLine parsed_line, int *icf, int *dcf) {
    int err;
    err = check_symbol_validity(symbol, parsed_line);
    if (err != 0) {
        return err;
    }

    if (parsed_line.type == DIRECTIVE_ENTRY) {
        symbol->is_entry = 1;
    }
    else if (parsed_line.type == DIRECTIVE_EXTERN) {
        symbol->is_external = 1;
    }
    else if (parsed_line.type == DIRECTIVE_DB || parsed_line.type == DIRECTIVE_DH ||
             parsed_line.type == DIRECTIVE_DW || parsed_line.type == DIRECTIVE_ASCIZ) {
        symbol->is_data = 1;
        symbol->value = *dcf;
    }
    else {
        symbol->is_code = 1;
        symbol->value = *icf;
    }

    return 0;
}

Symbol *find_symbol(char *name, Symbol **symbols) {
    Symbol *current = *symbols;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}


char *get_symbol_name(ParsedLine *parsed_line) {
    if (parsed_line->type == DIRECTIVE_ENTRY || parsed_line->type == DIRECTIVE_EXTERN) {
        return parsed_line->operands;
    }
    else {
        return parsed_line->label;
    }
}

Symbol *create_new_symbol(Symbol *new_symbol, char *symbol_name, Symbol **symbols) {
    new_symbol->is_code = 0;
    new_symbol->is_data = 0;
    new_symbol->is_external = 0;
    new_symbol->is_entry = 0;
    new_symbol->value = 0;
    new_symbol->next = NULL;
    strcpy(new_symbol->name, symbol_name);

    new_symbol->next = *symbols;
    *symbols = new_symbol;
    return new_symbol;
}

void update_all_data_symbols(Symbol *symbols, int icf) {
    Symbol *current;

    for (current = symbols; current != NULL; current = current->next) {
        if (current->is_data) {
            current->value += icf;
        }
    }
}

int create_symbol(ParsedLine parsed_line, Symbol **symbols, int *icf, int *dcf) {
    int err;
    char *symbol_name;
    Symbol *symbol;

    symbol_name = get_symbol_name(&parsed_line);

    /* this line gives no name to the table, like an instruction with no label */
    if (symbol_name[0] == '\0') {
        return 0;
    }
    
    symbol = find_symbol(symbol_name, symbols);
    if (symbol == NULL) {
        Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
        if (new_symbol == NULL) {
            return ERROR_MEMORY_ALLOCATION_FAILED;
        }
        symbol = create_new_symbol(new_symbol, symbol_name, symbols);
    }
    
    err = update_symbol(symbol, parsed_line, icf, dcf);
    if (err != 0) {
        return err;
    }
    return 0;
}
