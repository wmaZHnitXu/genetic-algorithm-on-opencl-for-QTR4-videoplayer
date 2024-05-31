#pragma once
#include <CL/cl.h>
#include <utils.h>

void loadAllTheOpenCLStuff();
void clearAllTheOpenCLStuff();
double invokeMseKernel(DMatrix* current, DMatrix* target);
Rect* invokeSingleRectKernel(DMatrix* current, DMatrix* target, int startFromLittle);
Rect** invoke256xRectKernel(DMatrix* current, DMatrix* target);
double invokeEvalKernel(Rect* rect, DMatrix* current, DMatrix* target);
void uploadCurrent(DMatrix* current);
void uploadTarget(DMatrix* target);