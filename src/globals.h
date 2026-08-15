/* globals.h - Constants and data structures shared by all stages of the assembler. */
#ifndef GLOBALS_H
#define GLOBALS_H

#define MAX_LABEL_LENGTH 31

#define MAX_LINE_LENGTH 80

#define INITIAL_IC 100

#define INITIAL_DC 0

#define MAX_DATA_BYTES 8192

#define OPERATIONS_AMOUNT 27

#define DIRECTIVES_AMOUNT 6

#define MIN_REGISTER 0

#define MAX_REGISTER 31

/* The immediate of an I type instruction is a signed field of 16 bits */
#define MIN_IMMEDIATE (-32768L)

#define MAX_IMMEDIATE 32767L

#endif
