#include <sequential_convert.h>
#include <stdlib.h>
#include <windows.h>
#include <opencl_stuff.h>

double mseMemMatrix[1024 * 1024];

double evalRectOnDMatrix(Rect *rect, DMatrix *current, DMatrix *target, double prevMse) {
    DMatrix *copyOfCurrent = allocDMatrix(current->cols, current->rows);

    for (int i = 0; i < current->rows; i++) {
        for (int j = 0; j < current->cols; j++) {
            setDMatrixElem(copyOfCurrent, i, j, getDMatrixElem(current, i, j));
        }
    } 

    drawRectOnDMatrix(rect, copyOfCurrent);
    double nextMse = mseBetweenDMatrixes(copyOfCurrent, target);
    freeDMatrix(copyOfCurrent);

    return prevMse - nextMse;
}

Node *getPopulation(DMatrix *current, DMatrix *target, int rectCount, int finalRectCount, double mse, int num) {

    struct Node* head = NULL;
    Rect *rect = NULL;

    for (int i = 0; i < rectCount; i++) {
        if (rect == NULL) rect = createRandomRect();
        else {
            rect->width = num > 12 ? (num > 512 ? rand5() : rand8()): rand255();
            rect->height = num > 12 ? (num > 512 ? rand5() : rand8()): rand255();
            rect->x = rand255();
            rect->y = rand255();
        }
        if (rect->x + rect->width > 256) rect->width = 256 - rect->x;
        if (rect->y + rect->height > 256) rect->height = 256 - rect->y;

        rect->color = getAvgColor(rect, target);
        rect->score = optimisedEvalRectOnMatrix(rect, current, target);
        
        if (head == NULL) {
            insertAtBeginning(&head, allocCopyOfRect(rect));
        }
        else {
            int count = 1;
            Node *prev = NULL;
            Node *cur = head;
            while (cur->rect->score > rect->score) {
                prev = cur;
                cur = cur->next;
                count++;
                if (cur == NULL || count >= finalRectCount) {
                    break;
                }
            }
            if (prev == NULL) {
                insertAtBeginning(&head, allocCopyOfRect(rect));
            }
            else {
                if (count < finalRectCount) {
                    prev->next = createNode(allocCopyOfRect(rect));
                    if (cur != NULL && cur != head) {
                        prev->next->next = cur;
                    }

                    while (countList(head) > finalRectCount) {
                        deleteLastNode(&head);
                    }
                }
            }
        }
    }

    free(rect);

    return head;
}

Node *getMutation(Node *prevGen, DMatrix *current, DMatrix *target, int childrenCount, int finalRectCount, double mse) {

    /*
    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
    double elapsedTime;
    // get ticks per second
    QueryPerformanceFrequency(&frequency);
    // start timer
    QueryPerformanceCounter(&t1);
    */

    struct Node *head = NULL;
    Node *currentParentNode = prevGen;

    Rect *rect = allocRect(0, 0, 0, 0, 0);

    while (currentParentNode != NULL) {
        Rect* prevRect = currentParentNode->rect;
        for (int j = 0; j < childrenCount; j++) {
            //MUTATION
            rect->x = prevRect->x + rand8() - 4;
            rect->y = prevRect->y + rand8() - 4;
            if (rect->x > 255) rect->x = 255;
            if (rect->y > 255) rect->y = 255;
            if (rect->x < 0) rect->x = 0;
            if (rect->y < 0) rect->y = 0;

            rect->width = prevRect->width + rand8() - 4;
            rect->height = prevRect->height + rand8() - 4;
            if (rect->x + rect->width > 256) rect->width = 256 - rect->x;
            if (rect->y + rect->height > 256) rect->height = 256 - rect->y;
            if (rect->width < 1) rect->width = 1;
            if (rect->height < 1) rect->height = 1;

            rect->color = getAvgColor(rect, target);
            rect->score = optimisedEvalRectOnMatrix(rect, current, target);
            
            if (head == NULL) {
                insertAtBeginning(&head, allocCopyOfRect(rect));
            }
            else {
                Node *prev = NULL;
                Node *cur = head;
                int count = 1;
                while (cur->rect->score > rect->score) {
                    prev = cur;
                    cur = cur->next;
                    count++;
                    if (cur == NULL || count >= finalRectCount) {
                        break;
                    }
                }
                if (prev == NULL) {
                    insertAtBeginning(&head, allocCopyOfRect(rect));
                }
                else {
                    if (count < finalRectCount) {
                        prev->next = createNode(allocCopyOfRect(rect));
                        if (cur != NULL && cur != head) {
                            prev->next->next = cur;
                        }

                        while (countList(head) > finalRectCount) {
                            deleteLastNode(&head);
                        }
                    }
                }
            }
        }

        currentParentNode = currentParentNode->next;
    }

    free(rect);

    /*
    QueryPerformanceCounter(&t2);
    // compute and print the elapsed time in millisec
    elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
    printf("Mutation done in %f ms.\n", elapsedTime);
    */

    return head;   
}

Rect *createRandomRect() {
    Rect *rect = allocRect(rand255(), rand255(), rand255(), rand255(), rand());
    return rect;
}

double mseBetweenDMatrixes(DMatrix* a, DMatrix* b) {
    double sum_sq = 0.0;
    double test = 0;
    if (a->cols != b->cols || a->rows != b->rows) {
        fprintf(stderr, "Dimensions do not match. %ix%i != %ix%i\n", a->cols, a->rows, b->cols, b->rows);
        exit(EXIT_FAILURE);
    }

    int diff[4];
    int width = a->cols;
    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < a->cols; j++) {
            
            unsigned char* aColors = &(a->data[i * width + j]);
            unsigned char* bColors = &(b->data[i * width + j]);
            for (int k = 0; k < 4; k++) {
                diff[k] = aColors[k] - bColors[k];
                diff[k] = diff[k] * diff[k];
            }
            double err = (double)(diff[0] + diff[1] + diff[2] + diff[3]);
            mseMemMatrix[i * width + j] = err;
            sum_sq += err;
            test += diff[3];
        }
    }

    double mse = sum_sq / (double)(a->cols * a->rows);
    return mse;
}

double optimisedEvalRectOnMatrix(Rect *rect, DMatrix *current, DMatrix *target) {
    float oldSum = 0.0;
    float newSum = 0.0;

    int w = target->cols;
    int h = target->rows;

    int x_start = rect->x;
    int x_end = rect->x + rect->width;
    x_end = x_end < w ? x_end : w;

    int y_start = rect->y;
    int y_end = rect->y + rect->height;
    y_end = y_end < h ? y_end : h;

    unsigned char* color = (unsigned char*)&rect->color;

    int width = target->cols;

    for (int i = y_start; i < y_end; i++) {
        for (int j = x_start; j < x_end; j++) {
            unsigned char* targetColor = &(target->data[i * width + j]);
            int diff[4];
            for (int k = 0; k < 4; k++) {
                diff[k] = color[k] - targetColor[k];
                diff[k] = diff[k] * diff[k];
            }
            float err = diff[0] + diff[1] + diff[2] + diff[3];

            newSum += err;
            oldSum += mseMemMatrix[i * width + j];
        }
    }

    float divider = current->cols * current->rows;
    float mse = (oldSum - newSum) / divider;
    return (double)mse;
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

    int channelSumms[4];
    for (int k = 0; k < 4; k++) channelSumms[k] = 0;
    unsigned char* test = NULL;
    int pxCount = 0;

    int width = target->cols;
    for (int i = y_start; i < y_end; i++) {
        for (int j = x_start; j < x_end; j++) {
            unsigned char* colors = &(target->data[i * width + j]);
            for (int k = 0; k < 4; k++) {
                channelSumms[k] += colors[k];
            }
            //printf("color:0x%08x\n", target->data[i][j]);
            test = colors;
            pxCount++;
        }
    }

    //0xFFFFFFFF
    //   3 2 1 0
    if (pxCount == 0) {
        return 0x00000000;
    }
    int r = channelSumms[2] / pxCount;
    int g = channelSumms[1] / pxCount;
    int b = channelSumms[0] / pxCount;
    //printf("r%ig%ib%i %f\n", channelSumms[2], channelSumms[1], channelSumms[0], g);

    int result = 0xFF000000;
    result += r * 0x00010000;
    result += g * 0x00000100;
    result += b * 0x00000001;
    return result;
}

Rect* getNextSequentialRect(DMatrix* currentMatrix, DMatrix* targetMatrix, int num) {
    num = 10000;
    int mutationsteps = 1;
    int childrencount = 10;
    Node* population = getPopulation(currentMatrix, targetMatrix, 100, 10, 0.0, num);
    for (int j = 0; j < mutationsteps; j++) {
        Node* madeittosex = createSublist(population, 10);
        freeList(population);

        population = getMutation(madeittosex, currentMatrix, targetMatrix, childrencount, 10, 0.0);
        freeList(madeittosex);
    }

    Rect* rectToApply = allocCopyOfRect(population->rect);
    freeList(population);

    return rectToApply;
}

int rand255() {
    return rand() % 256;
}

int rand8() {
    return rand() % 9;
}

int rand5() {
    return rand() % 9;
}

