#include <sequential_convert.h>
#include <stdlib.h>

Node *getPopulation(DMatrix *current, DMatrix *target, int rectCount) {
    for (int i = 0; i < rectCount; i++) {

    }
}

double evalRectOnDMatrix(Rect rect, DMatrix *current, DMatrix *target, double prevMse) {
    DMatrix *copyOfCurrent = allocDMatrix(current->rows, current->cols);

    for (int i = 0; i < current->rows; i++) {
        for (int j = 0; j < current->cols; j++) {
            copyOfCurrent->data[i][j] = current->data[i][j];
        }
    }

    double nextMse = mseBetweenDMatrixes(copyOfCurrent, target);
    freeDMatrix(copyOfCurrent);

    return prevMse - nextMse;
}

Rect *createRandomRect() {
    Rect *rect = allocRect(rand255(), rand255(), rand255(), rand255(), 0xFF0000FF);
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

