#include <stdio.h>

#include <CL/cl.h>
#include <image_presentation.h>
#include <utils.h>
#include <sequential_convert.h>
#include <windows.h>
#include <opencl_stuff.h>

int main()
{
    cl_int CL_err = CL_SUCCESS;
    cl_uint numPlatforms = 0;

    CL_err = clGetPlatformIDs( 0, NULL, &numPlatforms );

    if (CL_err == CL_SUCCESS)
        printf_s("%u platform(s) found\n", numPlatforms);
    else
        printf_s("clGetPlatformIDs(%i)\n", CL_err);
    
    printf_s("sex???");
    DMatrix* targetMatrix = createMatrixFromPng("test.png");
    DMatrix* currentMatrix = allocDMatrix(targetMatrix->cols, targetMatrix->rows);
    
    int rectcount = 3072;
    int mutationsteps = 3;
    int childrencount = 250;

    ///*
    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
    double elapsedTime;
    // get ticks per second
    QueryPerformanceFrequency(&frequency);
    //*/

    QueryPerformanceCounter(&t1);

    loadAllTheOpenCLStuff();
    double mseK = invokeMseKernel(currentMatrix, targetMatrix);
    clearAllTheOpenCLStuff();
    printf("Kernel returned mse:%f\n", mseK);

    double mse = mseBetweenDMatrixes(currentMatrix, targetMatrix);
    printf("Sequential mse:%f\n", mse);
    for (int i = 0; i < rectcount; i++) {

        if (i == rectcount - 230) {
            // start timer
            QueryPerformanceCounter(&t1);
        }

        Node* population = getPopulation(currentMatrix, targetMatrix, 1000, 10, mse, i);
        for (int j = 0; j < mutationsteps; j++) {
            Node* madeittosex = createSublist(population, 7);
            freeList(population);

            population = getMutation(madeittosex, currentMatrix, targetMatrix, childrencount, 10, mse);
            freeList(madeittosex);
        }

        Rect* rectToApply = allocCopyOfRect(population->rect);
        freeList(population);

        drawRectOnDMatrix(rectToApply, currentMatrix);
        mse = mseBetweenDMatrixes(currentMatrix, targetMatrix);
        printf_s("Rect#%i  MSE:%f color:0x%08x\n", i+1, mse, rectToApply->color);
        free(rectToApply);
    }

    ///*
    QueryPerformanceCounter(&t2);
    // compute and print the elapsed time in millisec
    elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
    printf("256 done in %f ms.\n", elapsedTime);
    //*/
    
    //drawRectOnDMatrix(allocRect(128, 128, 128, 128, 0xFFFF00FF), currentMatrix);
    
    displayMatrix(currentMatrix);

    freeDMatrix(currentMatrix);    
    return 0;
}