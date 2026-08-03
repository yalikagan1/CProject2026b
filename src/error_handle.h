typedef struct Error {
    int id;
    char *txt;
} Error;

typedef enum ERROR_CODES {
    ERROR_CODE_1 = 1,
    ERROR_CODE_2 = 2,
    ERROR_CODE_3 = 3,
    ERROR_CODE_4 = 4
} ERROR_CODES;

void print_error(int e, char *filename, int line);

void print_inner_error(int e);
