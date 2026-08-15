#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "second_pass.h"
#include "error_handle.h"
#include "helpers.h"
#include "symbols_table_handler.h"


static char *label_operand(Operation op) {
    if(op.format == OP_FORMAT_RRL) {
        return op.arg3;
    }
    if(op.format == OP_FORMAT_LABEL) {
        return op.arg1;
    }

    if(op.format == OP_FORMAT_JUMP && parse_register(op.arg1) == -1) {
        return op.arg1;
    }
    return NULL;
}

static int check_all_labels_defined(CodeImage *code, Symbol *symbols, char *filepath) {
    char *am_filename = add_file_extention(filepath, ".am");
    CodeImage *current;
    Symbol *target;
    char *label;
    int no_errors = 1;

    for(current = code; current != NULL; current = current->next) {
        label = label_operand(current->operation);
        if(label == NULL) {
            continue;
        }

        target = find_symbol(label, &symbols);

        if(target == NULL) {
            print_error(ERROR_UNDEFINED_SYMBOL, am_filename, current->line_number);
            no_errors = 0;
        }
        /* the immediate of a branch is a distance from here to the label, and
           there is no distance to a label that lives in another file */
        else if(current->operation.format == OP_FORMAT_RRL && target->is_external) {
            print_error(ERROR_EXTERNAL_BRANCH_TARGET, am_filename, current->line_number);
            no_errors = 0;
        }
    }

    free(am_filename);
    return no_errors;
}

int second_pass(char *am_filename, Symbol *symbols, CodeImage *code,
    DataImage *data, int *icf, int *dcf) {
    char* output_file = NULL;
    char hex_str[13];
    Operation op;
    unsigned int binary_code;
    int i, current_counter;
    FILE *file_write;
    CodeImage *current;

    if(!check_all_labels_defined(code, symbols, am_filename)) {
        return -1;
    }

    output_file = add_file_extention(am_filename, ".ob");
    file_write = fopen(output_file, "w");

    if(file_write == NULL) {
        print_error(ERROR_OPEN_FILE, output_file, 0);
        free(output_file);
        return -1;
    }

    /* write file header (ICF DCF)*/
    fprintf(file_write, "     %d ", *icf - INITIAL_IC);
    fprintf(file_write, "%d\n", *dcf);
 
    /* write code image (instructions), code itself stays on the head of the
       list because write_externals needs it later */
    current = code;
    while(current != NULL) {
        op = current->operation;
        current_counter = current->current_ic;
        binary_code = create_binary_code(op, symbols, current_counter);
        int_to_hex(binary_code, hex_str);
        fprintf(file_write, "%04d ", current_counter);
        fprintf(file_write, "%s\n", hex_str);
        current = current->next;
    }

    current_counter = *icf - 4;

    /* write data image */
    for(i = 0; i < data->count; i++) {
        if(i%4 == 0) {
            current_counter += 4;
            fprintf(file_write, "%04d", current_counter);
        }
        fprintf(file_write, " %02X", (unsigned char)data->bytes[i]);
        if(i%4 == 3)
            fprintf(file_write, "\n");
    }

    /* the last row of the data can hold less than four bytes */
    if(data->count % 4 != 0) {
        fprintf(file_write, "\n");
    }

    fclose(file_write);
    free(output_file);
    /* write .ent file */
    write_entries(am_filename, symbols);

    /* write .ext file*/
    write_externals(am_filename, symbols, code);
    return 0;
    }

void write_entries(char *am_filename, Symbol *symbols) {
    char* output_file = NULL;
    Symbol *head = symbols;
    FILE *file_write;
    int written = 0;

    output_file = add_file_extention(am_filename, ".ent");
    file_write = fopen(output_file, "w");

    if(file_write == NULL) {
        print_error(ERROR_OPEN_FILE, output_file, 0);
        free(output_file);
        return;
    }

    while (head != NULL) {
        if(head->is_entry) {
            fprintf(file_write, "%s ", head->name);
            fprintf(file_write, "%04d\n", head->value);
            written++;
        }
        head = head->next;
    }

    fclose(file_write);

    /* a file with no entries at all does not get a .ent */
    if(written == 0) {
        remove(output_file);
    }
    free(output_file);
}

void write_externals(char *am_filename, Symbol *symbols, CodeImage *code) {
    char* output_file = NULL;
    FILE *file_write;
    Operation op;
    int written = 0;

    output_file = add_file_extention(am_filename, ".ext");
    file_write = fopen(output_file, "w");

    if(file_write == NULL) {
        print_error(ERROR_OPEN_FILE, output_file, 0);
        free(output_file);
        return;
    }

    while(code != NULL) {
        op = code->operation;
        if(is_operand_external(op.arg1, symbols)) {
            fprintf(file_write, "%s ", op.arg1);
            fprintf(file_write, "%04d\n", code->current_ic);
            written++;
        }

        if(is_operand_external(op.arg2, symbols)) {
            fprintf(file_write, "%s ", op.arg2);
            fprintf(file_write, "%04d\n", code->current_ic);
            written++;
        }

        if(is_operand_external(op.arg3, symbols)) {
            fprintf(file_write, "%s ", op.arg3);
            fprintf(file_write, "%04d\n", code->current_ic);
            written++;
        }
        code = code->next;
    }

    fclose(file_write);

    /* a file with no external references at all does not get a .ext */
    if(written == 0) {
        remove(output_file);
    }
    free(output_file);
}

unsigned int create_binary_code(Operation op, Symbol *s, int current_ic) {
    if(op.type == INST_TYPE_R)
        return create_r_code(op);
    if(op.type == INST_TYPE_I)
        return create_i_code(op, s, current_ic);
    if(op.type == INST_TYPE_J)
        return create_j_code(op, s);
    return -1;
}

unsigned int create_r_code(Operation op) {
    int rs, rt, rd;
    rs = parse_register(op.arg1);

    /* the operations of opcode 1 take two registers only, and the second one
       of them is rd and not rt */
    if(op.opcode == 1) {
        rt = 0;
        rd = parse_register(op.arg2);
    } else {
        rt = parse_register(op.arg2);
        rd = parse_register(op.arg3);
    }

   return ((unsigned int)op.opcode << 26) |
          (rs << 21) |
          (rt << 16) |
          (rd << 11) |
          (op.funct << 6);
}

unsigned int create_i_code(Operation op, Symbol *s, int current_ic) {
    int rs, rt;
    short int imm;
    rs = parse_register(op.arg1);
    if(op.opcode < 15 || op.opcode > 18) {
        imm =(short int)atoi(op.arg2);
        rt = parse_register(op.arg3);    
    } else {
        rt = parse_register(op.arg2); 
        imm = (short int)(get_symbol_value(op.arg3, s) - current_ic);
    }
    
   return ((unsigned int)op.opcode << 26) |
          (rs << 21) |
          (rt << 16) |
          (imm & 0xFFFF);
}

unsigned int create_j_code(Operation op, Symbol *s) {
    int opcode, reg = 0, address = 0, temp_reg = 0;
    opcode = op.opcode;
    if(opcode == 30) {
        temp_reg = parse_register(op.arg1);
        if(temp_reg != -1) {
            reg = 1;
            address = temp_reg;
        }
    }

    if (reg == 0 && opcode != 63) {
        address = get_symbol_value(op.arg1, s);
    }

    return ((unsigned int)op.opcode << 26) |
    (reg << 25) |
    address;

}

int get_symbol_value(char *arg, Symbol *s) {
    Symbol *head = s;

    while (head != NULL) {
        if(strcmp(arg, head->name) == 0) {
            if(head->is_external)
                return 0;
            else
                return head->value;
        }
        head = head->next;
    }
    return -1;
}

void print_as_binary(int num) {
    int i;
    for (i = 31; i >= 0; i--) {
        int bit = (num >> i) & 1;
        printf("%d", bit);
        
        if (i % 8 == 0 && i != 0) {
            printf(" ");
        }
    }
    printf("\n");
}

void int_to_hex(unsigned int num, char *str) {
    int i, j, byte, digit;
    j = 0;

    for (i = 0; i < 4; i++) {
        byte = (num >> (i * 8)) & 255;

        if (i > 0) {
            str[j++] = ' ';
        }

        digit = (byte >> 4) & 15;
        str[j++] = (digit < 10) ? '0' + digit : 'A' + digit - 10;

        digit = byte & 15;
        str[j++] = (digit < 10) ? '0' + digit : 'A' + digit - 10;
    }

    str[j] = '\0';
}
