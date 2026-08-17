#include "globals.h"
#include "grammar.h"

/**
 * @brief The function is reading the input file that comes after thr preprocessor 
 and creating the symbols table, the data image and the code image, making the ground for the second pass.
 *
 * @param am_filename the string with the name of the file.
 * @param symbols a pointer to the symbols table.
 * @param code a pointer to the code image.
 * @param data a pointer to the data image.
 * @param icf a pointer to the instruction counter.
 * @param dcf a pointer to the data counter.
 * @return 1 if success, 0 if error
 */
int first_pass(char *am_filename, Symbol **symbols, CodeImage **code,
               DataImage *data, int *icf, int *dcf);
