#include "error_handle.h"
#include "code_image_handler.h"
#include "line_parser.h"
#include "grammar.h"
#include <string.h>
#include <stdlib.h>

/* Puts the node at the end of the list, so that the instructions stay in the
   order they were written in. Takes the address of the head, because the
   first node has to be written into the head itself */
static void add_node_to_code_image_at_the_end(CodeImage *node, CodeImage **code) {
    CodeImage *last_node;

    if (*code == NULL) {
        *code = node;
        return;
    }

    last_node = *code;
    while (last_node->next != NULL) {
        last_node = last_node->next;
    }

    last_node->next = node;
}


static void free_all_arguments(char *arg1, char *arg2, char *arg3) {
    free(arg1);
    free(arg2);
    free(arg3);
}

static char *copy_argument(char *token) {
    char *copy = malloc(strlen(token) + 1);

    if (copy == NULL) {
        return NULL;
    }

    strcpy(copy, token);
    return copy;
}

int extract_and_assume_arguments(char *operands, Operation *operation) {
    char *token;
    char *delimiter = ",";
    int count = 0;
    char *arg1 = NULL;
    char *arg2 = NULL;
    char *arg3 = NULL;
    char *copy;

    token = strtok(operands, delimiter);
    while (token != NULL) {
        count++;
        if (count > operation->arg_num) {
            free_all_arguments(arg1, arg2, arg3);
            return ERROR_TOO_MANY_OPERANDS;
        }

        copy = copy_argument(token);
        if (copy == NULL) {
            free_all_arguments(arg1, arg2, arg3);
            return ERROR_MEMORY_ALLOCATION_FAILED;
        }

        if (count == 1) {
            arg1 = copy;
        }
        if (count == 2) {
            arg2 = copy;
        }
        if (count == 3) {
            arg3 = copy;
        }

        token = strtok(NULL, delimiter);
    }

    if (count < operation->arg_num) {
        free_all_arguments(arg1, arg2, arg3);
        return ERROR_TOO_FEW_OPERANDS;
    }

    /* An operand the operation does not have stays NULL */
    operation->arg1 = arg1;
    operation->arg2 = arg2;
    operation->arg3 = arg3;

    return 0;
}

int add_instruction(ParsedLine parsed_line, CodeImage **code, int ic) {
    int err;
    CodeImage *node;
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

    node = (CodeImage *)malloc(sizeof(CodeImage));
    if (node == NULL) {
        free_all_arguments(new_operation.arg1, new_operation.arg2, new_operation.arg3);
        return ERROR_MEMORY_ALLOCATION_FAILED;
    }
    node->operation = new_operation;
    node->current_ic = ic;
    node->next = NULL;

    add_node_to_code_image_at_the_end(node, code);

    return 0;
}
