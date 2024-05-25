#include <utils.h>
#include <math.h>

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

Rect* allocRect(int x, int y, int width, int height, int color) {
    Rect* rect = (Rect*)malloc(sizeof(Rect));
    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
    rect->color = color;
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