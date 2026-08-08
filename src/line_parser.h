/* line_parser.h - Splits one line of assembly into the parts that the first
   pass works with. It only takes the line apart, it does not try to
   understand the operands. */
#ifndef LINE_PARSER_H
#define LINE_PARSER_H

#include "globals.h"

typedef struct {
    char label[MAX_LABEL_LENGTH + 1];   /* An empty string when the line has no label */
    char name[MAX_LINE_LENGTH + 1];     /* The operation name, or the directive with its dot */
    char *operands;                     /* Everything after the name, not splitted yet */
} ParsedLine;

/* Takes the line apart into the struct above. The operands field points into
   the line itself, so it is only good until the next line is read.
   Returns 0 when the line is fine, or the code of the error that was found */
int parse_line(char *line, ParsedLine *parsed);

#endif
