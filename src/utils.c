#include <utils.h>
#include <math.h>

// Function to create a new node
Node* createNode(int data) {
    Node* new_node = (struct Node*)malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    new_node->rect = data;
    new_node->next = NULL;
    return new_node;
}

// Function to insert a node at the beginning of the list
void insertAtBeginning(Node** head_ref, int new_data) {
    Node* new_node = create_node(new_data);
    new_node->next = *head_ref;
    *head_ref = new_node;
}

// Function to insert a node at the end of the list
void insertAtEnd(Node** head_ref, int new_data) {
    Node* new_node = create_node(new_data);
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

// Function to delete a node with a specific key
void deleteNode(Node** head_ref, int key) {
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

// Function to print the linked list
void printList(Node* node) {
    while (node != NULL) {
        printf("rect print not implemented");
        node = node->next;
    }
    printf("NULL\n");
}

// Function to free the entire linked list
void freeList(struct Node* head) {
    Node* current = head;
    Node* next;

    while (current != NULL) {
        next = current->next; // Save the next node
        free(current->rect);
        free(current);        // Free the current node
        current = next;       // Move to the next node
    }
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

Rect *allocCopyOfRect(Rect *original) {
    Rect* rect = (Rect*)malloc(sizeof(Rect));
    rect->x = original->x;
    rect->y = original->y;
    rect->width = original->width;
    rect->height = original->height;
    rect->color = original->color;
    return rect;
}

double mseBetweenDMatrixes(DMatrix* a, DMatrix* b) {
    double sum_sq = 0.0;
    if (a->cols != b->cols || a->rows != b->cols) {
        fprintf(stderr, "Dimensions do not match.\n");
        exit(EXIT_FAILURE);
    }

    int* diff = malloc(sizeof(int) * 4);
    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < a->cols; j++) {
            unsigned char* aColors = &(a->data[i][j]);
            unsigned char* bColors = &(b->data[i][j]);
            for (int k = 0; k < 4; k++) {
                diff[k] = aColors[k] - bColors[k];
                diff[k] = diff[k] * diff[k];
            }
            double err = sqrt(diff[0] + diff[1] + diff[2] + diff[3]);
            sum_sq += err * err;
        }
    }
    free(diff);

    double mse = sum_sq / (double)(a->cols * a->rows);
    return mse;

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
            matrix->data[i][j] = rect->color;
        }
    }
}