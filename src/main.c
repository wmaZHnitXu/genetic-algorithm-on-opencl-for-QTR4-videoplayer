#include <stdio.h>

#include <CL/cl.h>
#include <image_presentation.h>
#include <utils.h>
#include <sequential_convert.h>

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
    DMatrix* currentMatrix = allocDMatrix(256, 256);
    DMatrix* targetMatrix = createMatrixFromPng("test.png");
    
    int rectcount = 1024;
    int mutationsteps = 7;
    int childrencount = 100;

    for (int i = 0; i < rectcount; i++) {
        Node* population = getPopulation(currentMatrix, targetMatrix, 1000, 10);
        for (int j = 0; j < mutationsteps; j++) {
            Node* madeittosex = createSublist(population, 10);
            freeList(population);

            population = getMutation(madeittosex, currentMatrix, targetMatrix, childrencount, 10);
            freeList(madeittosex);
        }

        Rect* rectToApply = allocCopyOfRect(population->rect);
        freeList(population);

        drawRectOnDMatrix(rectToApply, currentMatrix);
        printf_s("Rect#%i  MSE:%f color:0x%08x\n", i+1, mseBetweenDMatrixes(currentMatrix, targetMatrix), rectToApply->color);
        free(rectToApply);
    }
    
    
    Rect* whole = allocRect(0, 0, 256, 256, 0x00000000);
    int color = getAvgColor(whole, targetMatrix);
    printf_s("color:0x%08x\n", color);
    //drawRectOnDMatrix(allocRect(128, 128, 128, 128, 0xFFFF00FF), currentMatrix);
    
    displayMatrix(currentMatrix);

    freeDMatrix(currentMatrix);    
    return 0;
}