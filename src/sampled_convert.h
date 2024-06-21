#pragma once
#include <sequential_convert.h>

struct DS256Stack {
    DMatrix* samples;
    double fullMse;
    int fullColor;
} typedef DS256Stack;

DS256Stack* createSampleStack(DMatrix* current, DMatrix target);