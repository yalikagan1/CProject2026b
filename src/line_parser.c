/* line_parser.c - Takes one line of assembly apart into our customized parsed line struct. */
#include <string.h>
#include <ctype.h>
#include "line_parser.h"
#include "error_handle.h"
#include "grammar.h"
#include "helpers.h"

/* Without change the original string, returns the length of the first word of the string, 
    that is up to the first space or to the end of the string. */
static int word_length(char *str) {
    int len = 0;

    while (str[len] != '\0' && !isspace((unsigned char)str[len])) {
        len++;
    }

    return len;
}

/* Checks a label name that was already cut out of the line, without its colon.
   Returns 0 when the name is legal, or the code of the error */
static int validate_label(char *label) {
    int i;

    if (!isalpha((unsigned char)label[0])) {
        return ERROR_INVALID_LABEL_NAME;
    }

    for (i = 1; label[i] != '\0'; i++) {
        if (!isalnum((unsigned char)label[i])) {
            return ERROR_INVALID_LABEL_NAME;
        }
    }

    if (is_reserved_word(label)) {
        return ERROR_LABEL_IS_RESERVED_WORD;
    }

    return 0;
}

int parse_line(char *line, ParsedLine *parsed) {
    int word_len;
    int err;

    parsed->label[0] = '\0';
    parsed->name[0] = '\0';
    parsed->operands = NULL;
    parsed->type = DIRECTIVE_NONE;

    word_len = word_length(line);

    if (line[0] == ':') {
        return ERROR_INVALID_LABEL_NAME;
    }

    if (word_len > 0 && line[word_len - 1] == ':') {
        if (word_len - 1 > MAX_LABEL_LENGTH) {
            return ERROR_LABEL_TOO_LONG;
        }

        strncpy(parsed->label, line, word_len - 1);
        parsed->label[word_len - 1] = '\0';

        err = validate_label(parsed->label);
        if (err) {
            return err;
        }

        line = skip_spaces(line + word_len);
        word_len = word_length(line);
    }

    /* A line that holds nothing but a label has nothing to assemble */
    if (word_len == 0) {
        return ERROR_MISSING_OPERATION;
    }

    strncpy(parsed->name, line, word_len);
    parsed->name[word_len] = '\0';

    if (parsed->name[0] == '.') {
        parsed->type = find_directive(parsed->name);

        if (parsed->type == DIRECTIVE_NONE) {
            return ERROR_UNKNOWN_DIRECTIVE;
        }
    }

    parsed->operands = skip_spaces(line + word_len);

    return 0;
}
