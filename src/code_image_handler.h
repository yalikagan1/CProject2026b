#ifndef CODE_IMAGE_HANDLER_H
#define CODE_IMAGE_HANDLER_H

#include "line_parser.h"
#include "grammar.h"

int add_instruction(ParsedLine parsed_line, CodeImage **code, int ic, int line_number);

/* Frees every instruction in the list and leaves the list empty */
void free_code_image(CodeImage **code);

#endif
