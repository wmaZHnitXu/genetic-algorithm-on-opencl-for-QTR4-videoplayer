#include <sequential_convert.h>
#include <stdlib.h>

double evalRectOnDMatrix(Rect *rect, DMatrix *current, DMatrix *target, double prevMse) {
    DMatrix *copyOfCurrent = allocDMatrix(current->rows, current->cols);

    for (int i = 0; i < current->rows; i++) {
        for (int j = 0; j < current->cols; j++) {
            copyOfCurrent->data[i][j] = current->data[i][j];
        }
    } 

    drawRectOnDMatrix(rect, copyOfCurrent);
    double nextMse = mseBetweenDMatrixes(copyOfCurrent, target);
    freeDMatrix(copyOfCurrent);

    return prevMse - nextMse;
}

Node *getPopulation(DMatrix *current, DMatrix *target, int rectCount) {
    double mse = mseBetweenDMatrixes(current, target);
    struct Node* head = NULL;

    for (int i = 0; i < rectCount; i++) {
        Rect *rect = createRandomRect();
        if (rect->x + rect->width > 256) rect->width = 256 - rect->x;
        if (rect->y + rect->height > 256) rect->height = 256 - rect->y;

        rect->color = getAvgColor(rect, target);
        rect->score = evalRectOnDMatrix(rect, current, target, mse);
        
        if (head == NULL) {
            insertAtBeginning(&head, rect);
        }
        else {
            Node *prev = NULL;
            Node *cur = head;
            while (cur->rect->score > rect->score) {
                prev = cur;
                cur = cur->next;
                if (cur == NULL) {
                    break;
                }
            }
            if (prev == NULL) {
                insertAtBeginning(&head, rect);
            }
            else {
                prev->next = createNode(rect);
                if (cur != NULL && cur != head) {
                    prev->next->next = cur;
                }
            }
        }
    }

    return head;
}

Node *getMutation(Node *prevGen, DMatrix *current, DMatrix *target, int childrenCount) {
    double mse = mseBetweenDMatrixes(current, target);
    struct Node *head = NULL;
    Node *currentParentNode = prevGen;

    while (currentParentNode != NULL) {
        for (int j = 0; j < childrenCount; j++) {
            Rect *rect = allocCopyOfRect(currentParentNode->rect);

            //MUTATION
            rect->x += rand31() - 16;
            rect->y += rand31() - 16;
            if (rect->x > 255) rect->x = 255;
            if (rect->y > 255) rect->y = 255;
            if (rect->x < 0) rect->x = 0;
            if (rect->y < 0) rect->y = 0;

            rect->width += rand31() - 16;
            rect->height += rand31() - 16;
            if (rect->x + rect->width > 256) rect->width = 256 - rect->x;
            if (rect->y + rect->height > 256) rect->height = 256 - rect->y;
            if (rect->width < 1) rect->width = 1;
            if (rect->height < 1) rect->height = 1;

            rect->color = getAvgColor(rect, target);
            rect->score = evalRectOnDMatrix(rect, current, target, mse);
            
            if (head == NULL) {
                insertAtBeginning(&head, rect);
            }
            else {
                Node *prev = NULL;
                Node *cur = head;
                while (cur->rect->score > rect->score) {
                    prev = cur;
                    cur = cur->next;
                    if (cur == NULL) {
                        break;
                    }
                }
                if (prev == NULL) {
                    insertAtBeginning(&head, rect);
                }
                else {
                    prev->next = createNode(rect);
                    if (cur != NULL && cur != head) {
                        prev->next->next = cur;
                    }
                }
            }
        }

        currentParentNode = currentParentNode->next;
    }

    return head;   
}

Rect *createRandomRect() {
    Rect *rect = allocRect(rand255(), rand255(), rand255(), rand255(), 0xFF0000FF);
    return rect;
}

int getAvgColor(Rect *rect, DMatrix *target) {
    int w = target->cols;
    int h = target->rows;

    int x_start = rect->x;
    int x_end = rect->x + rect->width;
    x_end = x_end < w ? x_end : w;

    int y_start = rect->y;
    int y_end = rect->y + rect->height;
    y_end = y_end < h ? y_end : h;

    unsigned char* channelSumms = malloc(sizeof(int));
    int pxCount = 0;

    for (int i = y_start; i < y_end; i++) {
        for (int j = x_start; j < x_end; j++) {
            unsigned char* colors = &(target->data[i][j]);
            for (int k = 0; k < 4; k++) {
                channelSumms[k] += colors[k];
            }
            pxCount++;
        }
    }

    double r = (double)channelSumms[1] / pxCount;
    double g = (double)channelSumms[2] / pxCount;
    double b = (double)channelSumms[3] / pxCount;

    free(channelSumms);

    int result =  0xFF000000;
    result += r * 0x00010000;
    result += g * 0x00000100;
    result += b * 0x00000001;

    return result;
}

int rand255() {
    return rand() % 256;
}

int rand31() {
    return rand() % 32;
}

