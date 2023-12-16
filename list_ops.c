#include <stdlib.h>

// Define the structure for linked list nodes
struct Node {
    struct Node *next;
    struct Node *prev;
    DCLANG_FLT data;  // New member for data
};

// Function to create a linked list with a single node containing user-chosen data
void listmakefunc() {
    // Allocate memory for the head node
    struct Node *list = (struct Node *)malloc(sizeof(struct Node));

    // Set the head node to point to itself in both directions
    list->next = list;
    list->prev = list;

    // Push the pointer to the head node onto the stack
    push((DCLANG_PTR)list);
}

// Function to append a node to the tail of the linked list
void listpushfunc() {
    if (data_stack_ptr < 2) {
        printf("lpush -- stack underflow; need <value> <list> on the stack! ");
        return;
    }

    // Pop args
    DCLANG_PTR list_ptr = (DCLANG_PTR)dclang_pop();
    DCLANG_FLT value = dclang_pop();

    // Convert pointers to the actual node structure
    struct Node *list = (struct Node *)list_ptr;

    // Allocate memory for the new node
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));

    // Initialize the new node
    new_node->data = value;

    // Insert the new node before the head node (at the tail)
    insque(new_node, list->prev);
}

// Function to pop a node from the tail of the linked list
void listpopfunc() {
    if (data_stack_ptr < 1) {
        printf("lpop -- stack underflow; need <list> on the stack! ");
        return;
    }

    // Pop arg
    DCLANG_PTR list_ptr = (DCLANG_PTR)dclang_pop();

    // Convert pointers to the actual node structure
    struct Node *list = (struct Node *)list_ptr;

    // Check if the list is empty
    if (list->next == list) {
        printf("lpop -- list is empty! ");
        return;
    }

    // Get the tail node
    struct Node *tail_node = list->prev;

    // Remove the tail node from the list
    remque(tail_node);

    // Push the data of the tail node onto the stack
    push((DCLANG_FLT)tail_node->data);

    // Free the memory of the popped node
    free(tail_node);
}

// Function to set the data of a node in the linked list
void listsetfunc() {
    if (data_stack_ptr < 3) {
        printf("l! -- stack underflow; need <value> <slot> <list> on the stack! ");
        return;
    }

    // Pop args
    DCLANG_PTR list_ptr = (DCLANG_PTR)dclang_pop();
    DCLANG_INT slot = (DCLANG_INT)dclang_pop();
    DCLANG_FLT value = dclang_pop();

    // Convert pointers to the actual node structure
    struct Node *list = (struct Node *)list_ptr;

    // Find the Nth node in the linked list
    struct Node *node = list;
    for (int i = 0; i < slot + 1; i++) {
        if (node->next == list) {
            // Reached the end of the list
            printf("l! -- slot out of bounds! ");
            return;
        }
        node = node->next;
    }

    // Set the data of the node
    node->data = value;
}

// Function to get the data of a node in the linked list
void listgetfunc() {
    if (data_stack_ptr < 2) {
        printf("l@ -- stack underflow; need <lost> <list> on the stack! ");
        return;
    }

    // Pop args
    DCLANG_PTR list_ptr = (DCLANG_PTR)dclang_pop();
    DCLANG_INT slot = (DCLANG_INT)dclang_pop();

    // Convert pointers to the actual node structure
    struct Node *list = (struct Node *)list_ptr;

    // Find the Nth node in the linked list
    struct Node *node = list;
    for (int i = 0; i < slot + 1; i++) {
        if (node->next == list) {
            // Reached the end of the list
            printf("l@ -- slot out of bounds! ");
            return;
        }
        node = node->next;
    }

    // Push the data of the node onto the stack
    push((DCLANG_FLT)node->data);
}


// Function to insert a node into a linked list before a specified node
void listinsertfunc() {
    if (data_stack_ptr < 3) {
        printf("lins -- stack underflow; need <value> <slot> <list> on the stack! ");
        return;
    }

    // Pop args
    DCLANG_PTR list_ptr = (DCLANG_PTR)dclang_pop();
    DCLANG_INT slot = (DCLANG_INT)dclang_pop();
    DCLANG_FLT value = dclang_pop();

    // Convert pointers to the actual node structure
    struct Node *list = (struct Node *)list_ptr;

    // Allocate memory for the new node
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));

    // Initialize the new node
    new_node->data = value;

    // Find the node before which to insert the new node
    struct Node *node_before = list;
    for (int i = 0; i < slot; i++) {
        if (node_before->next == list) {
            // Reached the end of the list
            printf("lins -- slot out of bounds! ");
            free(new_node);
            return;
        }
        node_before = node_before->next;
    }

    // Insert the new node before the specified node
    insque(new_node, node_before);
}

// Function to remove a node from a linked list at a specified node slot
void listremovefunc() {
    if (data_stack_ptr < 2) {
        printf("lrem -- stack underflow; need <slot> <list> on the stack! ");
        return;
    }

    // Pop args
    DCLANG_PTR list_ptr = (DCLANG_PTR)dclang_pop();
    DCLANG_INT slot = (DCLANG_INT)dclang_pop();

    // Convert pointers to the actual node structure
    struct Node *list = (struct Node *)list_ptr;

    // Find the node to remove
    struct Node *node_to_remove = list;
    for (int i = 0; i < slot + 1; i++) {
        if (node_to_remove->next == list) {
            // Reached the end of the list
            printf("lrem -- slot out of bounds! ");
            return;
        }
        node_to_remove = node_to_remove->next;
    }

    // Remove the specified node from the list
    remque(node_to_remove);

    // Free the memory of the removed node
    free(node_to_remove);
}

// Function to get the size (number of nodes) in a linked list
void listsizefunc() {
    if (data_stack_ptr < 1) {
        printf("lsize -- stack underflow; need <list> on the stack! ");
        return;
    }

    // Pop the list pointer from the stack
    DCLANG_PTR list_ptr = dclang_pop();

    // Convert pointer to the actual node structure
    struct Node *list = (struct Node *)list_ptr;

    // Initialize the size counter
    int size = 0;

    // Traverse the linked list and count each node
    struct Node *current = list->next;
    while (current != list) {
        size++;
        current = current->next;
    }

    // Push the size onto the stack
    push((DCLANG_INT)size);
}


// Function to delete (free) all nodes in a linked list
void listdeletefunc() {
    if (data_stack_ptr < 1) {
        printf("ldel -- stack underflow; need <list> on the stack! ");
        return;
    }

    // Pop the list pointer from the stack
    DCLANG_PTR list_ptr = (DCLANG_PTR)dclang_pop();

    // Convert pointer to the actual node structure
    struct Node *list = (struct Node *)list_ptr;

    // Traverse the linked list and free each node in the 'next' direction
    struct Node *current_next = list->next;
    while (current_next != list) {
        struct Node *next_node = current_next->next;
        free(current_next);
        current_next = next_node;
    }

    // Free the head node
    // Reset the list to a blank head node
    list->next = list;
    list->prev = list;
}
