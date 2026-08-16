#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "data_image_handler.h"
#include "error_handle.h"
#include "helpers.h"

static int size_of_directive(DirectiveType type) {
    if (type == DIRECTIVE_DB) {
        return 1;
    }
    if (type == DIRECTIVE_DH) {
        return 2;
    }

    return 4; /* only .dw is left*/
}

static int is_number_in_range(long value, int size_of_directive) {
    long limit = 1L << (size_of_directive * 8 - 1);

    return value >= -limit && value <= limit - 1;
}

static int write_number_to_data_image(DataImage *data, long value, int size_of_directive) {
    int i;

    if (data->count + size_of_directive > MAX_DATA_BYTES) {
        return ERROR_DATA_IMAGE_FULL;
    }

    for (i = 0; i < size_of_directive; i++) {
        data->bytes[data->count] = (unsigned char)((value >> (8 * i)) & 0xFF);
        data->count++;
    }

    return 0;
}


static int validate_next_number_and_assign_to_value(char **text, long *value, int size) {
    char *end;

    if (**text == ',') {
        return ERROR_INVALID_COMMAS;
    }

    *value = strtol(*text, &end, 10);

    if (end == *text) {
        return ERROR_INVALID_NUMBER;
    }

    /* only /0, , or space could be after number*/
    if (*end != '\0' && *end != ',' && !isspace((unsigned char)*end)) {
        return ERROR_INVALID_NUMBER;
    }

    *text = end;


    if (!is_number_in_range(*value, size)) {
        return ERROR_NUMBER_OUT_OF_RANGE;
    }

    return 0;
}


static int encode_numbers(char *operands, DirectiveType type, DataImage *data) {
    char *text = skip_spaces(operands);
    int size = size_of_directive(type);
    long value;
    int err;

    if (*text == '\0') {
        return ERROR_MISSING_OPERANDS;
    }

    while (*text != '\0') {
        err = validate_next_number_and_assign_to_value(&text, &value, size);
        if (err) {
            return err;
        }

        err = write_number_to_data_image(data, value, size);
        if (err) {
            return err;
        }

        text = skip_spaces(text);

        if (*text != '\0' && *text != ',') {
            return ERROR_INVALID_COMMAS;
        }

        if (*text == ',') {
            text = skip_spaces(text + 1);

            if (*text == '\0') {
                return ERROR_INVALID_COMMAS; /* list ended with a comma and no number after it */
            }
        }
    }

    return 0;
}

static char *check_quotes(char *text) {
    char *last_quote;

    if (*text != '"') { /* no opening quote*/
        return NULL;
    }

    text++;
    last_quote = strrchr(text, '"');
    if (last_quote == NULL) { /* no closing quote*/
        return NULL;
    }

    if (*skip_spaces(last_quote + 1) != '\0') { /* not the end of the string*/
        return NULL;
    }

    return last_quote;
}

static int encode_asciiz(char *operands, DataImage *data) {
    char *text = skip_spaces(operands);
    char *last_quote;
    int err;

    last_quote = check_quotes(text);
    if (last_quote == NULL) {
        return ERROR_INVALID_STRING;
    }

    text++;

    while (text < last_quote) {
        if (!isprint((unsigned char)*text)) {
            return ERROR_INVALID_STRING; /* not printable character*/
        }

        err = write_number_to_data_image(data, *text, 1);
        if (err) {
            return err;
        }

        text++;
    }

    return write_number_to_data_image(data, 0, 1);
}

int encode_data_directive(DirectiveType type, char *operands, DataImage *data) {
    if (type == DIRECTIVE_ASCIZ) {
        return encode_asciiz(operands, data);
    }

    if (type != DIRECTIVE_DB && type != DIRECTIVE_DH && type != DIRECTIVE_DW) {
        return ERROR_UNKNOWN_DIRECTIVE;
    }

    return encode_numbers(operands, type, data);
}
