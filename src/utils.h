#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <dmatrix.h>

double mseBetweenDMatrixes(DMatrix* a, DMatrix* b);

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int color;
    double score;
} Rect;

void drawRectOnDMatrix(Rect* rect, DMatrix* matrix);

Rect* allocRect(int x, int y, int width, int height, int color);

typedef struct {
    Rect* rect;
    struct Node* next;
} Node;

// Function prototypes

// Function to create a new node
struct Node* createNode(int data);

// Function to insert a node at the beginning of the list
void insertAtBeginning(struct Node** head_ref, int new_data);

// Function to insert a node at the end of the list
void insertAtEnd(struct Node** head_ref, int new_data);

// Function to delete a node with a specific key
void deleteNode(struct Node** head_ref, int key);

// Function to print the linked list
void printList(struct Node* node);

// Function to free the entire linked list
void freeList(struct Node* head);