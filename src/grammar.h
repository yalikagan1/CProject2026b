
typedef struct op_code {
    char *opcode;    /* The opcode corresponding to the operation */
    int arg_num;     /* The number of arguments for the operation */
} op_code;

int is_instruction(char *str);

int opcode_num(char *str);

int reg_num(char *str);
