#ifndef LINE_PARSER_H
#define LINE_PARSER_H

#include "globals.h"
#include "grammar.h"

typedef struct {
    char label[MAX_LABEL_LENGTH + 1];   /* An empty string when the line has no label */
    char name[MAX_LINE_LENGTH + 1];     /* The operation name, or the directive with its dot */
    char *operands;                     /* Everything after the name, not splitted yet */
    DirectiveType type;                 /* DIRECTIVE_NONE when the line is an instruction */
} ParsedLine;

/**
 * @brief Splits one line into the label, the name and the operands.
 *
 * This function splits one line of assembly into the label, the name and the operands.
 * The line has to be trimmed already, and not empty or a comment.
 * The operands field points into the line itself, so it is good only until the next line is read over it.
 * @param line the line to parse, it is not changed.
 * @param parsed the struct to fill, every field is written.
 * @return 0 if success, error code if error.
 */
int parse_line(char *line, ParsedLine *parsed);

#endif
