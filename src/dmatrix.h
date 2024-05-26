#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int rows;
    int cols;
    int* data;
} DMatrix;

DMatrix* allocDMatrix(int cols, int rows);

void freeDMatrix(DMatrix* m);

void setDMatrixElem(DMatrix* m, int row, int col, int value);
int getDMatrixElem(DMatrix* m, int row, int col);