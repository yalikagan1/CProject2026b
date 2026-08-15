/**
 * @brief validate macro name.
 *
 * This function validate macro name according to assembler rules.
 * @param macro_name string name of the macro.
 * @param filename name of the input file.
 * @param line_counter number of line in the input file.
 * @return 0 if valid, 1 if not valid.
 */
int validate_macro_name(char *macro_name, char *filename, int line_counter);

/**
 * @brief pre-process the assembler process.
 *
 * This function expand all macros in input file and creates .am file as output..
 * @param filename name of the input file.
 * @return 0 if valid, 1 if not valid.
 */
int main_macro_expand(char *filename);
