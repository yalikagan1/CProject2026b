typedef struct node {
    char *macro_name;         /* Name associated with the node */
    char *macro_content;      /* Content stored in the node */
    struct node *next;  /* Pointer to the next node in the linked list */
} node;

/**
 * @brief creates new node.
 *
 * This function creates new node.
 * @param macro_name string name of the new node.
 * @return pointer to the new node.
 */
node* create_node(char *macro_name);

/**
 * @brief adds node to the linked list.
 *
 * This function adds node to the linked list at it's end.
 * @param head pointer to the first node of the linked list.
 * @param new_node pointer to the new node to be added.
 */
void add_node(node *head, node *new_node);

/**
 * @brief free the linked list nodes.
 *
 * This function free the memory allocated in all linked list nodes.
 * @param head pointer to the first node of the linked list.
 */
void free_list(node *head);

/**
 * @brief print the linked list nodes.
 *
 * This function prints all linked list nodes.
 * @param head pointer to the first node of the linked list.
 */
void print_list(node *head);

/**
 * @brief search name of macro in linked list nodes.
 *
 * This function searches name of macro in linked list nodes.
 * @param head pointer to the first node of the linked list.
 * @param macro_name string name to be searched.
 * @return string of content of relevant node, NULL is didn't find.
 */
char* is_in_list(node *head, char *macro_name);
