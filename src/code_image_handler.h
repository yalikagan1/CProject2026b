#include "line_parser.h"
#include "grammar.h"

/**
 * @brief adds an instruction to the code image
 *
 * This function adds an instruction to the code image.
 * @param parsed_line the parsed line from the input file.
 * @param code a pointer to the code image.
 * @param ic the instruction counter.
 * @param line_number the line number of the instruction.
 * @return 0 if success, error code if error
 */
int add_instruction(ParsedLine parsed_line, CodeImage **code, int ic, int line_number);

/* Frees every instruction in the list and leaves the list empty */
void free_code_image(CodeImage **code);
