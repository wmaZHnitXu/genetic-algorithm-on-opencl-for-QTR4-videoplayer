#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <CL/cl.h>
#include <image_presentation.h>
#include <utils.h>
#include <sequential_convert.h>
#include <sequential_convert_experiment.h>
#include <windows.h>
#include <opencl_stuff.h>
#include <video_import.h>

static unsigned char clamp_to_uchar(int value) {
    if (value < 0) {
        return 0;
    }
    if (value > 255) {
        return 255;
    }
    return (unsigned char)value;
}

void writeRectToFile(FILE* file, Rect* rect) {
    if (file == NULL || rect == NULL) {
        return;
    }

    unsigned char bytes[7];

    int left = rect->x;
    int top = rect->y;
    int right = rect->x + rect->width;
    int down = rect->y + rect->height;

    unsigned char r = (rect->color >> 16) & 0xFF;
    unsigned char g = (rect->color >> 8) & 0xFF;
    unsigned char b = rect->color & 0xFF;

    bytes[0] = clamp_to_uchar(left);
    bytes[1] = clamp_to_uchar(top);
    bytes[2] = clamp_to_uchar(right);
    bytes[3] = clamp_to_uchar(down);
    bytes[4] = r;
    bytes[5] = g;
    bytes[6] = b;

    fwrite(bytes, sizeof(unsigned char), 7, file);
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_video_file> [output_data_file]\n", argv[0]);
        return -1;
    }

    const char* inputVideoFile = argv[1];
    char* outputDataFile = NULL;

    if (argc >= 3) {
        outputDataFile = argv[2];
    } else {

        const size_t len = strlen(inputVideoFile);
        outputDataFile = (char*)malloc(len + 5); //+5 for .dat & nullterm
        if (outputDataFile == NULL) {
            fprintf(stderr, "Failed to allocate memory for output file name.\n");
            return -1;
        }
        strcpy(outputDataFile, inputVideoFile);

        char* dot = strrchr(outputDataFile, '.');
        if (dot != NULL) {
            strcpy(dot, ".dat");
        } else {
            strcat(outputDataFile, ".dat");
        }
    }

    printf("Input video: %s\n", inputVideoFile);
    printf("Output data file: %s\n", outputDataFile);

    FILE* outputFile = fopen(outputDataFile, "wb");
    if (outputFile == NULL) {
        fprintf(stderr, "Error opening output file '%s' for writing.\n", outputDataFile);
        if (argc < 3) free(outputDataFile);
        return -1;
    }


    cl_int CL_err = CL_SUCCESS;
    cl_uint numPlatforms = 0;

    CL_err = clGetPlatformIDs( 0, NULL, &numPlatforms );

    loadAllTheOpenCLStuff();

    if (CL_err == CL_SUCCESS)
        printf_s("%u platform(s) found\n", numPlatforms);
    else
        printf_s("clGetPlatformIDs(%i)\n", CL_err);
    
    DMatrix* targetMatrix = createMatrixFromPng("test.png");
    
    DMatrix* g = createMatrixFromPng("g.png");
    double mse = mseBetweenDMatrixes(g, targetMatrix);
    printf("G MSE:%f", mse);
    //targetMatrix = allocDMatrix(256, 256);
    //drawRectOnDMatrix(allocRect(10, 10, 100, 100, 0xFF778822), targetMatrix);
    DMatrix* currentMatrix = allocDMatrix(targetMatrix->cols, targetMatrix->rows);
    
    int rectcount = 3072;
    int mutationsteps = 10;
    int childrencount = 26;

    mse = mseBetweenDMatrixes(currentMatrix, targetMatrix);

    FFmpegState state;
    if (initFFmpegState(&state, inputVideoFile) != 0) {
        fprintf(stderr, "Error initializing FFmpeg state.\n");
        fclose(outputFile);
        if (argc < 3) free(outputDataFile);
        return -1;
    }
    
    ///*256 Rects per kernel call
    
    LARGE_INTEGER frequency;        //ticks per second
    LARGE_INTEGER t1, t2;           //ticks
    double elapsedTime;
    //get ticks per second
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);

    DMatrix* targetMatrixFrame;

    int i = 0;
    for (; (targetMatrixFrame = readNextFrame(&state)) != NULL; i += 256) {

        if (currentMatrix->cols != targetMatrixFrame->cols || currentMatrix->rows != targetMatrixFrame->rows) {
            currentMatrix = allocDMatrix(targetMatrixFrame->cols, targetMatrixFrame->rows);
        }
        if (i > rectcount) {
            //i -= 256;
            //break;
        }

        for (int k = 0; k < 1; k++) {

            LARGE_INTEGER frequency;
            LARGE_INTEGER t1, t2;           
            double elapsedTime;
            QueryPerformanceFrequency(&frequency);
            QueryPerformanceCounter(&t1);

            Rect** rectsToApply = invoke256xRectKernel(currentMatrix, targetMatrixFrame);
            //printf_s("Rect#%i  MSE:%f color:0x%08x\n", i+1, 0.0, rectsToApply[255]->color);
            for (int j = 0; j < 256; j++) {
                writeRectToFile(outputFile, rectsToApply[j]);

                drawRectOnDMatrix(rectsToApply[j], currentMatrix);
                free(rectsToApply[j]);
            }

            QueryPerformanceCounter(&t2);
            elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
            printf("Kernel done in %f ms. Rect#%i\n", elapsedTime, i+1);

            displayMatrix(currentMatrix);
            free(rectsToApply);
        }
        freeDMatrix(targetMatrixFrame);
    }
    
    QueryPerformanceCounter(&t2);
    // compute and print the elapsed time in millisec
    elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
    printf("%i rects done in %f ms.\n", i, elapsedTime);

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

        
        Rect* sequentialRect = getNextSequentialRect(currentMatrix, targetMatrix, i);
        //Rect* kernelRect = invokeSingleRectKernel(currentMatrix, targetMatrix, 0);
        float kmse = 0.0f;//optimisedEvalRectOnMatrix(kernelRect, currentMatrix, targetMatrix);
        drawRectOnDMatrix(sequentialRect, currentMatrix);
        mse = mseBetweenDMatrixes(currentMatrix, targetMatrix);
        printf_s("Rect#%i  sMSE:%f kMSE:%f kSCR:%f MSE:%f\n", i+1, sequentialRect->score, kmse, 0.0, mse);
        displayMatrix(currentMatrix);
        free(sequentialRect);
        //free(kernelRect);
    }
    
    */

   /* EXPERIMENT

    printf("Sequential mse:%f\n", mse);
     LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
    double elapsedTime;
    // get ticks per second
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);

    DMatrix* originalMatrix = allocDMatrix(currentMatrix->cols, currentMatrix->rows);
    for (int i = 0; i < originalMatrix->rows * originalMatrix->cols; i++) {
        originalMatrix->data[i] = currentMatrix->data[i];
    }

    Node* head = NULL;
    Node* current = NULL;

    for (int i = 0; i < rectcount; i++) {
        
        Rect* sequentialRect = getNextSequentialRect(currentMatrix, targetMatrix, i);
        if (current == NULL) {
            head = createNode(sequentialRect);
            current = head;
        }
        else {
            current->next = createNode(sequentialRect);
            current = current->next;
        }
        //Rect* kernelRect = invokeSingleRectKernel(currentMatrix, targetMatrix, 0);
        float kmse = 0.0f;//optimisedEvalRectOnMatrix(kernelRect, currentMatrix, targetMatrix);
        drawRectOnDMatrix(sequentialRect, currentMatrix);
        mse = mseBetweenDMatrixes(currentMatrix, targetMatrix);
        printf_s("Rect#%i  sMSE:%f kMSE:%f kSCR:%f MSE:%f\n", i+1, sequentialRect->score, kmse, 0.0, mse);
        //displayMatrix(currentMatrix);
        //free(sequentialRect);
        //free(kernelRect);
    }

    for (int i = 0; i < 15; i++) {
        printf("mutation#%i cnt:%i\n", i, countList(head));
        mutateAllRects(head, originalMatrix, targetMatrix);
    }

    QueryPerformanceCounter(&t2);
    // compute and print the elapsed time in millisec
    elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
    printf("Conversion done in %f ms.\n", elapsedTime);

    Node* currentNode = head;
    for (int i = 0; currentNode != NULL; i++) {
        drawRectOnDMatrix(currentNode->rect, originalMatrix);
        currentNode = currentNode->next;
    }
    mse = mseBetweenDMatrixes(currentMatrix, targetMatrix);
    double smse = mseBetweenDMatrixes(originalMatrix, targetMatrix);
    printf("No shuffle mse:%f Shuffle mse:%f", mse, smse);
    */
    //drawRectOnDMatrix(allocRect(128, 128, 128, 128, 0xFFFF00FF), currentMatrix);

    for (int i = 0; i < 1000000; i++) {
        displayMatrix(currentMatrix);
    }

    fclose(outputFile);
    if (argc < 3) {
        free(outputDataFile);
    }
    
    freeDMatrix(currentMatrix); 
    clearAllTheOpenCLStuff();   
    return 0;
}