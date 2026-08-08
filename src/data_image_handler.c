/* data_image_handler.c - Turns the operands of .db .dh .dw and .asciz into
   bytes in the data image. The bytes are written in little endian, the low
   byte first, which is the order the machine reads them in. */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "data_image_handler.h"
#include "error_handle.h"
#include "helpers.h"

/* How many bytes one number of this directive takes */
static int size_of_directive(DirectiveType type) {
    if (type == DIRECTIVE_DB) {
        return 1;
    }
    if (type == DIRECTIVE_DH) {
        return 2;
    }

    /* Only .dw is left, because the caller turned away every other type */
    return 4;
}

/* Returns 1 when the number fits in a signed field of this many bytes */
static int is_number_in_range(long value, int size) {
    long limit = 1L << (size * 8 - 1);

    return value >= -limit && value <= limit - 1;
}

/* Puts one number in the data image. Returns 0 on success, or the code of the error */
static int write_number_to_data_image(DataImage *data, long value, int size) {
    int i;

    if (data->count + size > MAX_DATA_BYTES) {
        return ERROR_DATA_IMAGE_FULL;
    }

    for (i = 0; i < size; i++) {
        data->bytes[data->count] = (unsigned char)((value >> (8 * i)) & 0xFF);
        data->count++;
    }

    return 0;
}

/* Reads one number from the string and moves the pointer past it.
   Returns 0 on success, or the code of the error */
static int read_number(char **text, long *value) {
    char *end;

    *value = strtol(*text, &end, 10);

    /* Nothing was read at all, so this is not a number */
    if (end == *text) {
        return ERROR_INVALID_NUMBER;
    }

    /* only /0, , or space is allowed after the number*/
    if (*end != '\0' && *end != ',' && !isspace((unsigned char)*end)) {
        return ERROR_INVALID_NUMBER;
    }

    *text = end;

    return 0;
}

/* Writes a list of numbers separated by commas */
static int encode_numbers(char *operands, int size, DataImage *data) {
    char *text = skip_spaces(operands);
    long value;
    int err;

    /* Without this the loop below would report an invalid number, and send
       the reader looking for a bad number in a line that has none */
    if (*text == '\0') {
        return ERROR_MISSING_OPERANDS;
    }

    for (;;) {
        if (*text == ',') {
            return ERROR_INVALID_COMMAS; /* expecting a number first, Error*/
        }

        err = read_number(&text, &value);
        if (err) {
            return err;
        }

        if (!is_number_in_range(value, size)) {
            return ERROR_NUMBER_OUT_OF_RANGE;
        }

        err = write_number_to_data_image(data, value, size);
        if (err) {
            return err;
        }

        text = skip_spaces(text);
        if (*text == '\0') {
            return 0;
        }

        if (*text != ',') {
            return ERROR_INVALID_COMMAS;
        }

        text = skip_spaces(text + 1);

        /* The list ended with a comma and no number after it */
        if (*text == '\0') {
            return ERROR_INVALID_COMMAS;
        }
    }
}

/* Writes the characters of the string and the zero that closes it */
static int encode_string(char *operands, DataImage *data) {
    char *text = skip_spaces(operands);
    char *last_quote;
    int err;

    if (*text != '"') { /* no opening quote, Error*/
        return ERROR_INVALID_STRING;
    }

    text++;
    last_quote = strrchr(text, '"');
    if (last_quote == NULL) { /* no closing quote, Error*/
        return ERROR_INVALID_STRING;
    }

    if (*skip_spaces(last_quote + 1) != '\0') { /* not the end of the string, Error*/
        return ERROR_INVALID_STRING;
    }

    while (text < last_quote) {
        if (!isprint((unsigned char)*text)) { /* not printable character, Error*/
            return ERROR_INVALID_STRING;
        }

        err = write_number_to_data_image(data, *text, 1);
        if (err) {
            return err;
        }

        text++;
    }

    return write_number_to_data_image(data, 0, 1); /* write the zero that closes the string*/
}

int encode_data_directive(DirectiveType type, char *operands, DataImage *data) {
    if (type == DIRECTIVE_ASCIZ) {
        return encode_string(operands, data);
    }

    if (type != DIRECTIVE_DB && type != DIRECTIVE_DH && type != DIRECTIVE_DW) {
        return ERROR_UNKNOWN_DIRECTIVE;
    }

    return encode_numbers(operands, size_of_directive(type), data);
}
