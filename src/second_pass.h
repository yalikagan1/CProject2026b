#include "globals.h"
#include "grammar.h"

/**
 * @brief creates output files - .ob, .ext. .ent.
 *
 * This function creates the output files according to the input file's code and data images that created in first pass.
 * @param am_filename the string with the name of the file.
 * @param symbols the symbols table from the input file.
 * @param code a pointer to linked list of the code image.
 * @param data a pointer to linked list of the data image.
 * @param icf a pointer to the final instruction counter.
 * @param dcf a pointer to the final data counter.
 * @return 1 if success, -1 if any error
 */
int second_pass(char *am_filename, Symbol *symbols, CodeImage *code,
    DataImage *data, int *icf, int *dcf);

/**
 * @brief creates output file - .ent.
 *
 * This function creates the entries output file according to the input file's code and data images that created in first pass.
 * @param am_filename the string with the name of the file.
 * @param symbols the symbols table from the input file.
 */
void write_entries(char *am_filename, Symbol *symbols);

/**
 * @brief creates output file - .ext.
 *
 * This function creates the externals output file according to the input file's code and data images that created in first pass.
 * @param am_filename the string with the name of the file.
 * @param symbols the symbols table from the input file.
 * @param code a pointer to linked list of the code image.
 */
void write_externals(char *am_filename, Symbol *symbols, CodeImage *code);

/**
 * @brief encode operation to binary code (machine code)
 *
 * This function encode operation to binary code (machine code) according to assembler rules.
 * @param op the operation to be encoded.
 * @param s the symbols table from the input file.
 * @param current_ic value of operation's instruction counter.
 * @return the binary code as int.
 */
unsigned int create_binary_code(Operation op, Symbol *s, int current_ic);

/**
 * @brief encode R type operation to binary code (machine code)
 *
 * This function encode R type operation to binary code (machine code) according to assembler rules.
 * @param op the operation to be encoded.
 * @return the binary code as int.
 */
unsigned int create_r_code(Operation op);

/**
 * @brief encode I type operation to binary code (machine code)
 *
 * This function encode I type operation to binary code (machine code) according to assembler rules.
 * @param op the operation to be encoded.
 * @param s the symbols table from the input file.
 * @param current_ic value of operation's instruction counter.
 * @return the binary code as int.
 */
unsigned int create_i_code(Operation op, Symbol *s, int current_ic);

/**
 * @brief encode J type operation to binary code (machine code)
 *
 * This function encode J type operation to binary code (machine code) according to assembler rules.
 * @param op the operation to be encoded.
 * @param s the symbols table from the input file.
 * @return the binary code as int.
 */
unsigned int create_j_code(Operation op, Symbol *s);

/**
 * @brief returns the value of symbol.
 *
 * This function returns the value of symbol from the symbol table.
 * @param arg string name of the symbol
 * @param s the symbols table from the input file.
 * @return the value of the symbol, -1 if didn't find.
 */
int get_symbol_value(char *arg, Symbol *s);

void print_as_binary(int num);

/**
 * @brief encode number to hexadecimal.
 *
 * This function encodes number to hexadecimal according to assembler rules.
 * @param num number to be encoded.
 * @param str string that hex will be encoded to.
 */
void int_to_hex(unsigned int num, char *str);
