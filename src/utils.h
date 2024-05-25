#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dmatrix.h>

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int color;
    double score;
} Rect;

typedef struct Node {
    Rect* rect;
    struct Node* next; // Correctly referencing the Node struct
} Node;

// Function prototypes

// Function to create a new node
Node* createNode(Rect* rect);

// Function to insert a node at the beginning of the list
void insertAtBeginning(Node** head_ref, Rect* rect);

// Function to insert a node at the end of the list
void insertAtEnd(Node** head_ref, Rect* rect);

// Function to delete a node with a specific key
void deleteNode(Node** head_ref, Rect* key);

// Function to print the linked list
void printList(Node* node);

// Function to free the entire linked list
void freeList(Node* head);
int countList(Node* head);
Node* createSublist(Node* original, int count);

double mseBetweenDMatrixes(DMatrix* a, DMatrix* b);
void drawRectOnDMatrix(Rect* rect, DMatrix* matrix);
Rect* allocRect(int x, int y, int width, int height, int color);
Rect* allocCopyOfRect(Rect* original);
