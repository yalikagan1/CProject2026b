#include "error_handle.h"
#include "code_image_handler.h"
#include "line_parser.h"
#include "grammar.h"
#include <string.h>
#include <stdlib.h>


int extract_and_assume_arguments(char *operands, Operation *operation) {
    char *token;
    char *delimiter = ",";
    int count = 0;
    char *arg1 = NULL;
    char *arg2 = NULL;
    char *arg3 = NULL;

    token = strtok(operands, delimiter);
    while (token != NULL) {
        count++;
        if (count > operation->arg_num) {
            return ERROR_TOO_MANY_OPERANDS;
        }
        if (count == 1) {
            strcpy(arg1, token);
        }
        if (count == 2) {
            strcpy(arg2, token);
        }
        if (count == 3) {
            strcpy(arg3, token);
        }
        token = strtok(NULL, delimiter);
    }
    if (count < operation->arg_num) {
        return ERROR_TOO_FEW_OPERANDS;
    }
    operation->arg1 = malloc(sizeof(char) * (strlen(arg1) + 1));
    operation->arg2 = malloc(sizeof(char) * (strlen(arg2) + 1));
    operation->arg3 = malloc(sizeof(char) * (strlen(arg3) + 1));
    strcpy(operation->arg1, arg1);
    strcpy(operation->arg2, arg2);
    strcpy(operation->arg3, arg3);
    return 0;
}

int add_instruction(ParsedLine parsed_line, CodeImage *code) {
    int err;
    const Operation *operation = find_operation(parsed_line.name);
    Operation new_operation;

    if (operation == NULL) {
        return ERROR_MISSING_OPERATION;
    }

    new_operation.name = operation->name;
    new_operation.opcode = operation->opcode;
    new_operation.funct = operation->funct;
    new_operation.type = operation->type;
    new_operation.format = operation->format;
    new_operation.arg_num = operation->arg_num;
    err = extract_and_assume_arguments(parsed_line.operands, &new_operation);
    if (err) {
        return err;
    }

    code->next->current = new_operation;
    code->next->next = NULL;
    code->next = code->next->next;

    return 0;
}
