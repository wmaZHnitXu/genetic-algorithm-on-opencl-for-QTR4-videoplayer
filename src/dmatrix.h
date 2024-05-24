#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int rows;
    int cols;
    int** data;
} DMatrix;

DMatrix* allocDMatrix(int rows, int cols);

void freeDMatrix(DMatrix* m);

void setDMatrixElem(DMatrix* m, int row, int col, int value);