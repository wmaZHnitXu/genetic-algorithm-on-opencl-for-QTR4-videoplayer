#include <stdio.h>

#include <CL/cl.h>
#include <image_presentation.h>
#include <utils.h>
#include <sequential_convert.h>
#include <windows.h>
#include <opencl_stuff.h>
#include <video_import.h>

int main()
{
    cl_int CL_err = CL_SUCCESS;
    cl_uint numPlatforms = 0;

    CL_err = clGetPlatformIDs( 0, NULL, &numPlatforms );

    loadAllTheOpenCLStuff();

    if (CL_err == CL_SUCCESS)
        printf_s("%u platform(s) found\n", numPlatforms);
    else
        printf_s("clGetPlatformIDs(%i)\n", CL_err);
    
    printf_s("sex???");
    DMatrix* targetMatrix = createMatrixFromPng("test.png");
    //targetMatrix = allocDMatrix(256, 256);
    //drawRectOnDMatrix(allocRect(10, 10, 100, 100, 0xFF778822), targetMatrix);
    DMatrix* currentMatrix = allocDMatrix(targetMatrix->cols, targetMatrix->rows);
    
    int rectcount = 1024;
    int mutationsteps = 10;
    int childrencount = 26;

    double mse = mseBetweenDMatrixes(currentMatrix, targetMatrix);

    printf_s("sex???\n");
    FFmpegState state;
    if (initFFmpegState(&state, "turbo.mp4") != 0) {
        fprintf(stderr, "Error initializing FFmpeg state.\n");
        return -1;
    }
    printf_s("sex???\n");
    
    ///* 256 Rects per kernel call
    

    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
    double elapsedTime;
    // get ticks per second
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);

    DMatrix* targetMatrixFrame;

    for (int i = 0; (targetMatrixFrame = readNextFrame(&state)) != NULL; i += 256) {
        //if (i > rectcount) break;

        for (int k = 0; k < 1; k++) {
        Rect** rectsToApply = invoke256xRectKernel(currentMatrix, targetMatrixFrame);
        printf_s("Rect#%i  MSE:%f color:0x%08x\n", i+1, 0.0, rectsToApply[255]->color);
        for (int j = 0; j < 256; j++) {
            drawRectOnDMatrix(rectsToApply[j], currentMatrix);
            free(rectsToApply[j]);
        }

        displayMatrix(currentMatrix);
        free(rectsToApply);

        }

        freeDMatrix(targetMatrixFrame);

        
        
        
    }

    
    QueryPerformanceCounter(&t2);
    // compute and print the elapsed time in millisec
    elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
    printf("%i rects done in %f ms.\n", rectcount, elapsedTime);

    //*/

    //*/

    /* EvalComparison

    for (int i = 0; i < 10; i++) {
        Rect* randomRect = createRandomRect();
        randomRect->color = getAvgColor(randomRect, targetMatrix);
        int seqColor = randomRect->color;
        double seq = optimisedEvalRectOnMatrix(randomRect, currentMatrix, targetMatrix);
        double par = invokeEvalKernel(randomRect, currentMatrix, targetMatrix);
        printf("seq:%f 0x%08x\n", seq, seqColor);
        printf("par:%f 0x%08x\n", par, randomRect->color);
    }

    */

    /* GpuProcessing single rect

    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
    double elapsedTime;
    // get ticks per second
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);

    for (int i = 0; i < rectcount; i++) {
        
        Rect* rect = invokeSingleRectKernel(currentMatrix, targetMatrix, i > 10);
        rect->color = getAvgColor(rect, targetMatrix);
        drawRectOnDMatrix(rect, currentMatrix);
        printf_s("Rect#%i  score:%f color:0x%08x\n", i+1, mseBetweenDMatrixes(currentMatrix, targetMatrix), rect->color);
        free(rect);
        displayMatrix(currentMatrix);

    }

    QueryPerformanceCounter(&t2);
    // compute and print the elapsed time in millisec
    elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
    printf("Conversion done in %f ms.\n", elapsedTime);

    //*/

    /* SEQUENTIAL
    printf("Sequential mse:%f\n", mse);

    for (int i = 0; i < rectcount; i++) {

        
        Rect* sequentialRect = getNextSequentialRect(currentMatrix, targetMatrix);
        Rect* kernelRect = invokeSingleRectKernel(currentMatrix, targetMatrix, 0);
        float kmse = optimisedEvalRectOnMatrix(kernelRect, currentMatrix, targetMatrix);
        drawRectOnDMatrix(kernelRect, currentMatrix);
        mse = mseBetweenDMatrixes(currentMatrix, targetMatrix);
        printf_s("Rect#%i  sMSE:%f kMSE:%f kSCR:%f MSE:%f\n", i+1, sequentialRect->score, kmse, kernelRect->score, mse);
        displayMatrix(currentMatrix);
        free(sequentialRect);
        free(kernelRect);
    }
    
    */
    
    //drawRectOnDMatrix(allocRect(128, 128, 128, 128, 0xFFFF00FF), currentMatrix);

    freeDMatrix(currentMatrix); 
    clearAllTheOpenCLStuff();   
    return 0;
}