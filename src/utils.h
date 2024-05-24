#include <stdio.h>
#include <stdlib.h>
#include <dmatrix.h>

struct Node {
    int data;
    struct Node* next;
};

// Function prototypes

// Function to create a new node
struct Node* create_node(int data);

// Function to insert a node at the beginning of the list
void insert_at_beginning(struct Node** head_ref, int new_data);

// Function to insert a node at the end of the list
void insert_at_end(struct Node** head_ref, int new_data);

// Function to delete a node with a specific key
void delete_node(struct Node** head_ref, int key);

// Function to print the linked list
void print_list(struct Node* node);

// Function to free the entire linked list
void free_list(struct Node* head);

float mseBetweenDMatrixes(DMatrix* a, DMatrix* b);

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int color;
} Rect;