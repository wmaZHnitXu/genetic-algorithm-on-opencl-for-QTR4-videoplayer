#include <dmatrix.h>
#include <stdio.h>

DMatrix* allocDMatrix(int cols, int rows) {
    DMatrix* m = (DMatrix*)malloc(sizeof(DMatrix));
    m->rows = rows;
    m->cols = cols;
    m->data = (int**)malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++) {
        m->data[i] = (int*)malloc(cols * sizeof(int));
        for (int j = 0; j < cols; j++) {
            m->data[i][j] = 0x00000000;
        }
    }
    return m;
}

void freeDMatrix(DMatrix* m) {
    for (int i = 0; i < m->rows; i++) {
        free(m->data[i]);
    }
    free(m->data);
    free(m);
}

void setDMatrixElem(DMatrix* m, int row, int col, int value) {
    m->data[row][col] = value;
}

int getDMatrixElem(DMatrix* m, int row, int col) {
    return m->data[row][col];
}

DMatrix* addDMatrices(DMatrix* a, DMatrix* b) {
    if (a->rows != b->rows || a->cols != b->cols) {
        printf("Error: Matrices have different dimensions.\n");
        return NULL;
    }
    DMatrix* c = allocDMatrix(a->rows, a->cols);
    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < a->cols; j++) {
            c->data[i][j] = a->data[i][j] + b->data[i][j];
        }
    }
    return c;
}
