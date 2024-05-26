#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <utils.h>

Node* createNode(Rect* rect) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    new_node->rect = rect;
    new_node->next = NULL;
    return new_node;
}

void insertAtBeginning(Node** head_ref, Rect* rect) {
    Node* new_node = createNode(rect);  // Correct function call
    new_node->next = *head_ref;
    *head_ref = new_node;
}

void insertAtEnd(Node** head_ref, Rect* rect) {
    Node* new_node = createNode(rect);  // Correct function call
    if (*head_ref == NULL) {
        *head_ref = new_node;
        return;
    }
    Node* temp = *head_ref;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new_node;
}

void deleteNode(Node** head_ref, Rect* key) {
    Node* temp = *head_ref;
    Node* prev = NULL;

    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->rect == key) {
        *head_ref = temp->next;
        free(temp->rect);
        free(temp);
        return;
    }

    // Search for the key to be deleted, keep track of the previous node
    while (temp != NULL && temp->rect != key) {
        prev = temp;
        temp = temp->next;
    }

    // If key was not present in the list
    if (temp == NULL) return;

    // Unlink the node from the linked list
    prev->next = temp->next;
    free(temp->rect);
    free(temp);
}

void deleteLastNode(Node** head_ref) {
    if (*head_ref == NULL) {
        return;
    }

    Node* temp = *head_ref;

    if (temp->next == NULL) {
        free(temp->rect);
        free(temp);
        *head_ref = NULL;
        return;
    }

    Node* prev = NULL;
    while (temp->next != NULL) {
        prev = temp;
        temp = temp->next;
    }

    prev->next = NULL;

    free(temp->rect);
    free(temp);
}

// Function to print the linked list
void printList(Node* node) {
    int elem = 0;
    while (node != NULL) {
        printf("%i score:%f\n", elem++, node->rect->score);
        node = node->next;
    }
    printf("NULL\n");
}

// Function to free the entire linked list
void freeList(Node* head) {
    Node* current = head;
    Node* next;

    while (current != NULL) {
        next = current->next; // Save the next node
        free(current->rect);
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }
}

int countList(Node* head) {
    Node* current = head;
    int count = 0;

    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

Node* createSublist(Node* original, int count) {
    if (count == 0) return NULL;

    Node* current = original;
    Node* head = createNode(allocCopyOfRect(original->rect));
    Node* newCurrent = head;

    for (int i = 1; i < count && current->next != NULL; i++) {
        current = current->next;
        newCurrent->next = createNode(allocCopyOfRect(current->rect));
        newCurrent = newCurrent->next;
    }

    return head;
}

Rect* allocRect(int x, int y, int width, int height, int color) {
    Rect* rect = (Rect*)malloc(sizeof(Rect));
    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
    rect->color = color;
    return rect;
}

Rect* allocCopyOfRect(Rect* original) {
    Rect* rect = (Rect*)malloc(sizeof(Rect));
    rect->x = original->x;
    rect->y = original->y;
    rect->width = original->width;
    rect->height = original->height;
    rect->color = original->color;
    rect->score = original->score;
    return rect;
}

void drawRectOnDMatrix(Rect* rect, DMatrix* matrix) {
    int w = matrix->cols;
    int h = matrix->rows;

    int x_start = rect->x;
    int x_end = rect->x + rect->width;
    x_end = x_end < w ? x_end : w;

    int y_start = rect->y;
    int y_end = rect->y + rect->height;
    y_end = y_end < h ? y_end : h;

    for (int i = y_start; i < y_end; i++) {
        for (int j = x_start; j < x_end; j++) {
            int width = matrix->cols;
            matrix->data[i * width + j] = rect->color;
        }
    }
}
