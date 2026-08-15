#include "error_handle.h"
#include "code_image_handler.h"
#include "line_parser.h"
#include "grammar.h"
#include "helpers.h"
#include <string.h>
#include <stdlib.h>

static int validate_operands(char *arg1, char *arg2, char *arg3, Operation *operation) {
    switch (operation->format) {
        case OP_FORMAT_RRR:
            if (parse_register(arg1) == -1 || parse_register(arg2) == -1 || parse_register(arg3) == -1) {
                return ERROR_INVALID_OPERAND;
            }
            break;
        case OP_FORMAT_RR:
            if (parse_register(arg1) == -1 || parse_register(arg2) == -1) {
                return ERROR_INVALID_OPERAND;
            }
            break;
        case OP_FORMAT_RIR:
            if (parse_register(arg1) == -1 || !is_number(arg2) || parse_register(arg3) == -1) {
                return ERROR_INVALID_OPERAND;
            }
            break;
        case OP_FORMAT_RRL:
            if (parse_register(arg1) == -1 || parse_register(arg2) == -1 || validate_label(arg3) != 0) {
                return ERROR_INVALID_OPERAND;
            }
            break;
        case OP_FORMAT_JUMP:
            /* jmp takes a label or a register */
            if (parse_register(arg1) == -1 && validate_label(arg1) != 0) {
                return ERROR_INVALID_OPERAND;
            }
            break;
        case OP_FORMAT_LABEL:
            if (validate_label(arg1) != 0) {
                return ERROR_INVALID_OPERAND;
            }
            break;
        case OP_FORMAT_NONE: /* hlt has no operands */
            break;
        default:
            return ERROR_INVALID_FORMAT;
    }

    return 0;
}

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

static int commas_validation(char *operands) {
    char *text = skip_spaces(operands);

    /* the list may not open with a comma */
    if (*text == ',') {
        return ERROR_INVALID_COMMAS;
    }

    while (*text != '\0') {
        if (*text == ',') {
            text = skip_spaces(text + 1);
            /* after a comma there has to be another operand */
            if (*text == '\0' || *text == ',') {
                return ERROR_INVALID_COMMAS;
            }
        }
        else {
            text++;
        }
    }
    return 0;
}

int extract_and_assume_arguments(char *operands, Operation *operation) {
    char *token;
    char *delimiter = ",";
    int count = 0;
    char *arg1 = NULL;
    char *arg2 = NULL;
    char *arg3 = NULL;
    char *copy;
    int err;

    err = commas_validation(operands);
    if (err) {
        return err;
    }

    token = strtok(operands, delimiter);
    while (token != NULL) {
        token = trim_whitespaces(token);
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

int add_instruction(ParsedLine parsed_line, CodeImage **code, int ic, int line_number) {
    int err;
    CodeImage *node;
    const Operation *operation = find_operation(parsed_line.name);
    Operation new_operation;

    if (operation == NULL) {
        return ERROR_UNKNOWN_OPERATION;
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

    err = validate_operands(new_operation.arg1, new_operation.arg2, new_operation.arg3, &new_operation);
    if (err) {
        free_all_arguments(new_operation.arg1, new_operation.arg2, new_operation.arg3);
        return err;
    }

    node = (CodeImage *)malloc(sizeof(CodeImage));
    if (node == NULL) {
        free_all_arguments(new_operation.arg1, new_operation.arg2, new_operation.arg3);
        return ERROR_MEMORY_ALLOCATION_FAILED;
    }

    node->operation = new_operation;
    node->current_ic = ic;
    node->line_number = line_number;
    node->next = NULL;

    add_node_to_code_image_at_the_end(node, code);

    return 0;
}

void free_code_image(CodeImage **code) {
    CodeImage *current = *code;
    CodeImage *next;

    while (current != NULL) {
        next = current->next;
        free_all_arguments(current->operation.arg1, current->operation.arg2,
                           current->operation.arg3);
        free(current);
        current = next;
    }

    *code = NULL;
}
