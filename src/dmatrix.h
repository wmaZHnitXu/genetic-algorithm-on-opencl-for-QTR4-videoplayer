#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int rows;
    int cols;
    int* data;
} DMatrix;

typedef struct {
    int rows;
    int cols;
    double* data;
} EMatrix;

DMatrix* allocDMatrix(int cols, int rows);
EMatrix* allocEMatrix(int cols, int rows);

void freeDMatrix(DMatrix* m);
void freeEMatrix(EMatrix* m);

void setDMatrixElem(DMatrix* m, int row, int col, int value);
void setEMatrixElem(EMatrix* m, int row, int col, double value);
int getDMatrixElem(DMatrix* m, int row, int col);
double getEMatrixElem(EMatrix* m, int row, int col);