#pragma once
#include <CL/cl.h>
#include <utils.h>

void loadAllTheOpenCLStuff();
void clearAllTheOpenCLStuff();
double invokeMseKernel(DMatrix* current, DMatrix* target);