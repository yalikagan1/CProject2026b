#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "second_pass.h"
#include "error_handle.h"
#include "helpers.h"

int second_pass(char *am_filename, Symbol *symbols, CodeImage *code,
    DataImage *data, int *icf, int *dcf) {
    char* output_file = NULL;
    Operation op;
    int binary_code;
    Symbol *head = symbols;
    FILE *file_write;

    output_file = add_file_extention(am_filename, ".ent");
    file_write = fopen(output_file, "w");

    if(file_write == NULL) {
        print_error(ERROR_OPEN_FILE, am_filename, 0);
        return -1;
    }

    /* write file header (ICF DCF)*/
    fprintf(file_write, "     %d ", *icf);
    fprintf(file_write, "%d\n", *dcf);

    /* write code image (instructions) */
    while(code != NULL) {
        op = code->current;
        binary_code = create_binary_code(op, symbols, code->current_ic);
        // TODO : create HEX
        // TODO : write to file
    }

    /* write data image */

    /* write .ent file */
    write_entries(am_filename, symbols);

    /* write .ext file*/
    write_externals(am_filename, symbols, code);
    }

void write_entries(char *am_filename, Symbol *symbols) {
    char* output_file = NULL;
    Symbol *head = symbols;
    FILE *file_write;

    output_file = add_file_extention(am_filename, ".ent");
    file_write = fopen(output_file, "w");

    if(file_write == NULL) {
        print_error(ERROR_OPEN_FILE, am_filename, 0);
        return;
    }

    while (head != NULL) {
        if(head->is_entry) {
            fprintf(file_write, "%s ", head->name);
            fprintf(file_write, "%04d\n", head->value);
        }
        head = head->next;
    }

    fclose(file_write);
}

void write_externals(char *am_filename, Symbol *symbols, CodeImage *code) {
    char* output_file = NULL;
    FILE *file_write;
    Operation op;

    output_file = add_file_extention(am_filename, ".ext");
    file_write = fopen(output_file, "w");

    if(file_write == NULL) {
        print_error(ERROR_OPEN_FILE, am_filename, 0);
        return;
    }

    while(code != NULL) {
        op = code->current;
        if(is_operand_external(op.arg1, symbols)) {
            fprintf(file_write, "%s ", op.arg1);
            fprintf(file_write, "%04d\n", code->current_ic);
        }

        if(is_operand_external(op.arg2, symbols)) {
            fprintf(file_write, "%s ", op.arg2);
            fprintf(file_write, "%04d\n", code->current_ic);
        }

        if(is_operand_external(op.arg3, symbols)) {
            fprintf(file_write, "%s ", op.arg3);
            fprintf(file_write, "%04d\n", code->current_ic);
        }
        code = code->next;
    }

    fclose(file_write);
}

int create_binary_code(Operation op, Symbol *s, int current_ic) {
    if(op.type == INST_TYPE_R)
        return create_r_code(op);
    if(op.type == INST_TYPE_I)
        return create_i_code(op, current_ic);
    if(op.type == INST_TYPE_J)
        return create_j_code(op, s);
    return -1;
}

int create_r_code(Operation op) {
    int rs, rt, rd;
    rs = parse_register(op.arg1);
    rt = parse_register(op.arg2);
    rd = parse_register(op.arg3);

    if(op.opcode == 1)
        rt = 0;

   return (op.opcode << 26) |
          (rs << 21) |
          (rt << 16) |
          (rd << 11) |
          (op.funct << 6);
}

int create_i_code(Operation op, int current_ic) {
    int rs, rt;
    short int imm;
    rs = parse_register(op.arg1);
    if(op.opcode < 15 || op.opcode > 18) {
        imm =(short int)atoi(op.arg2);
        rt = parse_register(op.arg3);    
    } else {
        rt = parse_register(op.arg2); 
        imm = (short int)atoi(op.arg3);
        imm = imm - current_ic;
    }
    
   return (op.opcode << 26) |
          (rs << 21) |
          (rt << 16) |
          (imm & 0xFFFF);
}

int create_j_code(Operation op, Symbol *s) {
    int opcode, reg = 0, address = 0, temp_reg = 0;
    opcode = op.opcode;
    if(opcode == 30) {
        temp_reg = parse_register(op.arg1);
        if(temp_reg != -1) {
            reg = 1;
            address = temp_reg;
        }
    }
    if (opcode != 63) {
        address = get_symbol_value(op.arg1, s);
    }

    return (op.opcode << 26) |
    (reg << 25) |
    (address << 24);

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
    for (int i = 31; i >= 0; i--) {
        int bit = (num >> i) & 1;
        printf("%d", bit);
        
        // מוסיף רווח כל 8 ביטים (למעט בסוף)
        if (i % 8 == 0 && i != 0) {
            printf(" ");
        }
    }
    printf("\n");
}