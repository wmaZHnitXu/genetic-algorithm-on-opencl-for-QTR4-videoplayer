#include <dmatrix.h>
#include <stdio.h>

DMatrix* allocDMatrix(int cols, int rows) {
    DMatrix* m = (DMatrix*)malloc(sizeof(DMatrix));
    m->rows = rows;
    m->cols = cols;
    m->data = (int*)malloc(rows * sizeof(int*) * cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            m->data[i * cols + j] = 0x00000000;
        }
    }
    return m;
}

void freeDMatrix(DMatrix* m) {
    free(m->data);
    free(m);
}

void setDMatrixElem(DMatrix* m, int row, int col, int value) {
    int width = m->cols;
    m->data[row * width + col] = value;
}

int getDMatrixElem(DMatrix* m, int row, int col) {
    int width = m->cols;
    return m->data[row * width + col];
}

EMatrix* allocEMatrix(int cols, int rows) {
    EMatrix* m = (EMatrix*)malloc(sizeof(EMatrix));
    m->rows = rows;
    m->cols = cols;
    m->data = (double*)malloc(rows * sizeof(double*) * cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            m->data[i * cols + j] = 0.0;
        }
    }
    return m;
}

void freeEMatrix(EMatrix* m) {
    free(m->data);
    free(m);
}

void setEMatrixElem(EMatrix* m, int row, int col, double value) {
    int width = m->cols;
    m->data[row * width + col] = value;
}

double getEMatrixElem(EMatrix* m, int row, int col) {
    int width = m->cols;
    return m->data[row * width + col];
}

