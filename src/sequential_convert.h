#pragma once
#include <utils.h>
#include <dmatrix.h>

Node *getPopulation(DMatrix *current, DMatrix *target, int rectCount);
Node *getMutation(Node *prevGen, DMatrix *current, DMatrix *target, int childrenCount);
double evalRectOnDMatrix(Rect *rect, DMatrix *current, DMatrix *target, double prevMse);
Rect *createRandomRect();