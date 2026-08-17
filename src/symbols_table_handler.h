#ifndef SYMBOLS_TABLE_HANDLER_H
#define SYMBOLS_TABLE_HANDLER_H

#include "line_parser.h"
#include "grammar.h"

/**
 * @brief Puts into the symbols table the name this line gives.
 *
 * The main function that creates a new symbol in the symbols table.
 * The name is the label the line opens with, or the operand of .entry and .extern. A line
 * that names nothing succeeds and adds nothing; a name already there is updated, not replaced.
 * @param parsed_line the parsed line from the input file.
 * @param symbols a pointer to the symbols table.
 * @param icf a pointer to the instruction counter.
 * @param dcf a pointer to the data counter.
 * @param line_number the line number of the symbol.
 * @return 0 if success, error code if error
 */
int create_symbol(ParsedLine parsed_line, Symbol **symbols, int *icf, int *dcf, int line_number);

/**
 * @brief Updating and adding icf to all data symbols in the symbols table at the end of the first pass.
 *
 * This function updates all data symbols in the symbols table at the end of the first pass, adding to them icf.
 * @param symbols a pointer to the symbols table.
 * @param icf the instruction counter.
 */
void update_all_data_symbols(Symbol *symbols, int icf);

/**
 * @brief Finding a symbol in the symbols table by its name.
 *
 * This function finds a symbol in the symbols table by its name.
 * @param name the name of the symbol.
 * @param symbols a pointer to the symbols table.
 * @return the symbol if found, NULL if not found
 */
Symbol *find_symbol(char *name, Symbol **symbols);

/**
 * @brief Checking if all the entries are defined in the file.
 *
 * This function checks if all the entries are defined in the file, at the end of the first pass.
 * @param symbols a pointer to the symbols table.
 * @param filename the name of the file.
 * @return 1 if all the entries are fine, 0 if at least one is not
 */
int check_all_entries_defined(Symbol *symbols, char *filename);

/**
 * @brief Frees every symbol in the table and leaves the table empty.
 *
 * This function frees every symbol in the table and leaves the table empty at the end of the program.
 * @param symbols a pointer to the symbols table.
 */
void free_symbols_table(Symbol **symbols);

#endif
