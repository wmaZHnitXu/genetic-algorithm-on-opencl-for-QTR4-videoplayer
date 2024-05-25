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
    
    int rectcount = 128;
    int mutationsteps = 7;
    int childrencount = 100;

    
    for (int i = 0; i < rectcount; i++) {
        Node* population = getPopulation(currentMatrix, targetMatrix, 1000);
        for (int j = 0; j < 3; j++) {
            Node* madeittosex = createSublist(population, 10);
            freeList(population);

            population = getMutation(madeittosex, currentMatrix, targetMatrix, childrencount);
            freeList(madeittosex);
        }

        Rect* rectToApply = allocCopyOfRect(population->rect);
        freeList(population);

        drawRectOnDMatrix(rectToApply, currentMatrix);
        free(rectToApply);
        printf_s("Rect#%i  MSE:%f\n", i+1, mseBetweenDMatrixes(currentMatrix, targetMatrix));
    }
    
    
    displayMatrix(targetMatrix);

    freeDMatrix(currentMatrix);    
    return 0;
}