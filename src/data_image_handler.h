/* data_image_handler.h - The only module that writes into the data image.
   Only .db .dh .dw and .asciz get here, the other directives do not put
   anything in memory. */
#ifndef DATA_IMAGE_HANDLER_H
#define DATA_IMAGE_HANDLER_H

#include "globals.h"
#include "grammar.h"

/* Writes the numbers, or the string, that the directive holds into the data
   image, and moves its counter forward.
   Returns 0 when the operands are fine, or the code of the error */
int encode_data_directive(DirectiveType type, char *operands, DataImage *data);

#endif
