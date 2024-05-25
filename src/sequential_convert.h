#pragma once
#include <utils.h>
#include <dmatrix.h>

Node *getPopulation(DMatrix *current, DMatrix *target, int rectCount, int finalRectCount);
double mseBetweenDMatrixes(DMatrix* a, DMatrix* b);
Node *getMutation(Node *prevGen, DMatrix *current, DMatrix *target, int childrenCount, int finalRectCount);
double evalRectOnDMatrix(Rect *rect, DMatrix *current, DMatrix *target, double prevMse);
double optimisedEvalRectOnMatrix(Rect *rect, DMatrix *current, DMatrix *target, double prevMse);
int getAvgColor(Rect *rect, DMatrix *target);
Rect *createRandomRect();