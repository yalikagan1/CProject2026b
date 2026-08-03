typedef struct node {
    char *macro_name;         /* Name associated with the node */
    char *macro_content;      /* Content stored in the node */
    struct node *next;  /* Pointer to the next node in the linked list */
} node;

node* create_node(char *macro_name);

void add_node(node *head, node *new_node);

void free_list(node *head);

void print_list(node *head);

char* is_in_list(node *head, char *macro_name);
