#pragma once
#include <CL/cl.h>
#include <utils.h>

void loadAllTheOpenCLStuff();
void clearAllTheOpenCLStuff();
double invokeMseKernel(DMatrix* current, DMatrix* target);
Rect* invokeKernel(DMatrix* current, DMatrix* target, int startFromLittle);
double invokeEvalKernel(Rect* rect, DMatrix* current, DMatrix* target);
void uploadCurrent(DMatrix* current);
void uploadTarget(DMatrix* target);