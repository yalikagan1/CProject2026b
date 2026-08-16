#include "globals.h"
#include "grammar.h"

/**
 * @brief The function writes encoded numbers or string into the data image.
 *
 * This function writes encoded numbers or string into the data image.
 * @param type the type of the directive.
 * @param operands the operands of the directive.
 * @param data a pointer to the data image.
 * @return 0 if success, error code if error
 */
int encode_data_directive(DirectiveType type, char *operands, DataImage *data);
