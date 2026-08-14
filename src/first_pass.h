/* first_pass.h - The first pass of the assembler over one source file. */
#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "globals.h"
#include "grammar.h"

/* Runs the first pass over the .am file: fills the symbol table, the data
   image and the code image, and returns the final IC and DC through icf and dcf. 
   Returns 1 if the file had no errors at all, 0 otherwise. */
int first_pass(char *am_filename, Symbol **symbols, CodeImage **code,
               DataImage *data, int *icf, int *dcf);

#endif
