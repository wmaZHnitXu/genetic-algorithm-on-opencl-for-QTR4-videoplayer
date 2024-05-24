#include <utils.h>

// Function to create a new node
struct Node* create_node(int data) {
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    if (!new_node) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

// Function to insert a node at the beginning of the list
void insert_at_beginning(struct Node** head_ref, int new_data) {
    struct Node* new_node = create_node(new_data);
    new_node->next = *head_ref;
    *head_ref = new_node;
}

// Function to insert a node at the end of the list
void insert_at_end(struct Node** head_ref, int new_data) {
    struct Node* new_node = create_node(new_data);
    if (*head_ref == NULL) {
        *head_ref = new_node;
        return;
    }
    struct Node* temp = *head_ref;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new_node;
}

// Function to delete a node with a specific key
void delete_node(struct Node** head_ref, int key) {
    struct Node* temp = *head_ref;
    struct Node* prev = NULL;

    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->data == key) {
        *head_ref = temp->next;
        free(temp);
        return;
    }

    // Search for the key to be deleted, keep track of the previous node
    while (temp != NULL && temp->data != key) {
        prev = temp;
        temp = temp->next;
    }

    // If key was not present in the list
    if (temp == NULL) return;

    // Unlink the node from the linked list
    prev->next = temp->next;
    free(temp);
}

// Function to print the linked list
void print_list(struct Node* node) {
    while (node != NULL) {
        printf("%d -> ", node->data);
        node = node->next;
    }
    printf("NULL\n");
}

// Function to free the entire linked list
void free_list(struct Node* head) {
    struct Node* current = head;
    struct Node* next;

    while (current != NULL) {
        next = current->next; // Save the next node
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }
}

Rect* allocateRect(int x, int y, int width, int height, int color) {
    Rect* rect = (Rect*)malloc(sizeof(Rect));
    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
    rect->color = color;
    return rect;
}

float mseBetweenDMatrixes(DMatrix* a, DMatrix* b) {
    float err = 0.0f;
    if (a->cols != b->cols || a->rows != b->cols) {
        fprintf(stderr, "Dimensions does not match.\n");
        exit(EXIT_FAILURE);
    }

    
}