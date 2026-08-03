#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macros_linked_list.h"

node* create_node(char *macro_name) {
    node *new_node = (node*)malloc(sizeof(node));
    new_node->macro_name = macro_name;
    new_node->macro_content = NULL;
    new_node->next = NULL;
    return new_node;
}

void add_node(node *head, node *new_node) {
    node *current = head;
    while(current->next != NULL) 
        current = current->next;
    current->next = new_node;
}

void free_list(node *head) {
    node *current = head;
    while (current != NULL) {
        node *next = current->next;
        free(current->macro_name);
        free(current->macro_content);
        free(current);
        current = next;
    }
}

void print_list(node *head) {
    node *current = head;
    while (current != NULL) {
        printf("name = %s\t content = %s\n\n\n", current->macro_name, current->macro_content);
        current = current->next;
    }
}

char* is_in_list(node *head, char *macro_name) {
    node *current = head;
    while (current != NULL) {
        if (strcmp(current->macro_name, macro_name) == 0)
            return current->macro_content;
        current = current->next;
    }
    return NULL;
}
