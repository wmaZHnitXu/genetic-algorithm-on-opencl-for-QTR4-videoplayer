#include <opencl_stuff.h>
#include <windows.h>

cl_device_id s_device = NULL;
cl_context s_context = NULL;
cl_program s_program = NULL;
cl_kernel s_kernel = NULL;
cl_command_queue s_queue = NULL;


char* readKernelSource(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Could not open kernel file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    rewind(file);

    char* source = (char*)malloc(length + 1);
    if (!source) {
        fprintf(stderr, "Could not allocate memory for kernel source\n");
        exit(EXIT_FAILURE);
    }

    fread(source, 1, length, file);
    source[length] = '\0';
    fclose(file);

    return source;
}

cl_device_id createDevice() {
    cl_platform_id platform;
    cl_device_id device;
    cl_int err = 0;
    err |= clGetPlatformIDs(1, &platform, NULL);
    err |= clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1 , &device, NULL);
    if (err == CL_DEVICE_NOT_FOUND) {
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
    }
    if (err) {
        printf("err:0x%x in createDevice", err);
    }
    return device;
}

cl_context createContext(cl_device_id* pDevice) {
    int err = 0;
    cl_context context = clCreateContext(NULL, 1, pDevice, NULL, NULL, &err);
    if (err) {
        printf("err 0x%x in createContext", err);
        exit(1);
    }
    return context;
}

cl_program buildProgram(cl_context ctx, cl_device_id device)  {
    int ret = 0;

    const char* src = readKernelSource("kernel.cl");
    size_t srcLength = strlen(src);

    cl_program program = clCreateProgramWithSource(ctx, 1, (const char**)&src, (const size_t*)&srcLength, &ret);

    ret = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

    if (ret != CL_SUCCESS) {
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char* log = (char*)malloc(log_size);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        printf("Build log:\n%s\n", log);
        free(log);
        exit(1);
    }
    return program;
}

void loadAllTheOpenCLStuff() {
    int err = 0;
    s_device = createDevice();
    s_context = createContext(&s_device);
    s_program = buildProgram(s_context, s_device);
    s_kernel = clCreateKernel(s_program, "calculate_next_256_rects", &err);
    s_queue = clCreateCommandQueue(s_context, s_device, 0, &err);

    if (err) {
        printf("err 0x%x in load", err);
    }
}

Rect* invokeSingleRectKernel(DMatrix* current, DMatrix* target, int startFromLittle) {
    int ret = 0;
    int width = current->cols;
    int height = current->rows;
    int* result = malloc(sizeof(int) * 8 * 8);
    for (int i = 0; i < 64; i++) {
        result[i] = 0;
    }

    cl_mem currentMatrixBuffer = clCreateBuffer(s_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, current->cols * current->rows * sizeof(int), current->data, &ret);
    cl_mem targetMatrixBuffer = clCreateBuffer(s_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, target->cols * target->rows * sizeof(int), target->data, &ret);
    cl_mem rectArray = clCreateBuffer(s_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * 8 * 8, result, &ret);

    int randomSeed = rand();

    ret = clSetKernelArg(s_kernel, 0, sizeof(cl_mem), (void*)&currentMatrixBuffer);
    ret = clSetKernelArg(s_kernel, 1, sizeof(cl_mem), (void*)&targetMatrixBuffer);
    ret = clSetKernelArg(s_kernel, 2, sizeof(cl_mem), (void*)&rectArray);
    ret = clSetKernelArg(s_kernel, 3, sizeof(int), (void*)&width);
    ret = clSetKernelArg(s_kernel, 4, sizeof(int), (void*)&height);
    ret = clSetKernelArg(s_kernel, 5, sizeof(int), (void*)&randomSeed);
    ret = clSetKernelArg(s_kernel, 6, sizeof(int), (void*)&startFromLittle);

    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
    double elapsedTime;
    // get ticks per second
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);


    size_t globalItemSize[] = {32, 32};
    ret = clEnqueueNDRangeKernel(s_queue, s_kernel, 2, NULL, globalItemSize, NULL, 0, NULL, NULL);
    clFinish(s_queue);

    QueryPerformanceCounter(&t2);
    // compute and print the elapsed time in millisec
    elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
    printf("Rect kernel done in %f ms.\n", elapsedTime);

    ret = clEnqueueReadBuffer(s_queue, rectArray, CL_TRUE, 0, sizeof(int) * 8 * 8, result, 0, NULL, NULL);
    printf("%i\n", result[0]);

    clReleaseMemObject(rectArray);

    int resultColor = 0xFF000000;
    resultColor += 0x00010000 * result[4 + 8];
    resultColor += 0x00000100 * result[5 + 8];
    resultColor += 0x00000001 * result[6 + 8];

    for (int i = 0; i < 8; i++) {
        printf("result %i\n", result[i + 8]);
    }

    Rect* finalResult = allocRect(result[0 + 8], result[1 + 8], result[2 + 8], result[3 + 8], resultColor);
    finalResult->score = (double)result[7 + 8] * 10.0;
    free(result);

    return finalResult;
}

Rect** invoke256xRectKernel(DMatrix* current, DMatrix* target) {
    int ret = 0;
    int width = current->cols;
    int height = current->rows;
    int* result = malloc(sizeof(int) * 8 * 256);
    for (int i = 0; i < 2048; i++) {
        result[i] = 0;
    }

    int* test = malloc(sizeof(int) * 8 * 256);
    for (int i = 0; i < 8 * 256; i++) {
        test[i] = 0;
    }

    cl_mem currentMatrixBuffer = clCreateBuffer(s_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, current->cols * current->rows * sizeof(int), current->data, &ret);
    cl_mem targetMatrixBuffer = clCreateBuffer(s_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, target->cols * target->rows * sizeof(int), target->data, &ret);
    cl_mem intermediate = clCreateBuffer(s_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * 8 * 8, result, &ret);
    cl_mem rectArray = clCreateBuffer(s_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * 8 * 256, test, &ret);

    int randomSeed = rand();

    ret = clSetKernelArg(s_kernel, 0, sizeof(cl_mem), (void*)&currentMatrixBuffer);
    ret = clSetKernelArg(s_kernel, 1, sizeof(cl_mem), (void*)&targetMatrixBuffer);
    ret = clSetKernelArg(s_kernel, 2, sizeof(cl_mem), (void*)&intermediate);
    ret = clSetKernelArg(s_kernel, 3, sizeof(cl_mem), (void*)&rectArray);
    ret = clSetKernelArg(s_kernel, 4, sizeof(int), (void*)&width);
    ret = clSetKernelArg(s_kernel, 5, sizeof(int), (void*)&height);
    ret = clSetKernelArg(s_kernel, 6, sizeof(int), (void*)&randomSeed);


    size_t globalItemSize[] = {16, 16};
    ret = clEnqueueNDRangeKernel(s_queue, s_kernel, 2, NULL, globalItemSize, NULL, 0, NULL, NULL);
    clFinish(s_queue);

    ret = clEnqueueReadBuffer(s_queue, rectArray, CL_TRUE, 0, sizeof(int) * 8 * 256, result, 0, NULL, NULL);
    printf("%i\n", result[2]);

    clReleaseMemObject(rectArray);

    Rect** finalResult = malloc(sizeof(Rect*) * 256);
    

    for (int i = 0; i < 256; i++) {
        int offset = i * 8;

        int resultColor = 0xFF000000;
        resultColor += 0x00010000 * result[offset + 4];
        resultColor += 0x00000100 * result[offset + 5];
        resultColor += 0x00000001 * result[offset + 6];

        finalResult[i] = allocRect(
            result[offset],
            result[offset + 1],
            result[offset + 2],
            result[offset + 3],
            resultColor
        );
    }

    return finalResult;
}

double invokeMseKernel(DMatrix* current, DMatrix* target) {
    int ret = 0;
    int width = current->cols;
    int height = current->rows;

    

    cl_mem currentMatrixBuffer = clCreateBuffer(s_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, current->cols * current->rows * sizeof(int), current->data, &ret);
    cl_mem targetMatrixBuffer = clCreateBuffer(s_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, target->cols * target->rows * sizeof(int), target->data, &ret);
    cl_mem mseSumBuffer = clCreateBuffer(s_context, CL_MEM_WRITE_ONLY, sizeof(int), NULL, &ret);

    int initial_result = 0;
    ret = clEnqueueWriteBuffer(s_queue, mseSumBuffer, CL_TRUE, 0, sizeof(int), &initial_result, 0, NULL, NULL);

    ret = clSetKernelArg(s_kernel, 0, sizeof(cl_mem), (void*)&currentMatrixBuffer);
    ret = clSetKernelArg(s_kernel, 1, sizeof(cl_mem), (void*)&targetMatrixBuffer);
    ret = clSetKernelArg(s_kernel, 2, sizeof(cl_mem), (void*)&mseSumBuffer);
    ret = clSetKernelArg(s_kernel, 3, sizeof(int), (void*)&width);
    ret = clSetKernelArg(s_kernel, 4, sizeof(int), (void*)&height);


    size_t globalItemSize[] = {256, 256};
    ret = clEnqueueNDRangeKernel(s_queue, s_kernel, 2, NULL, globalItemSize, NULL, 0, NULL, NULL);
    clFinish(s_queue);

    int result;
    ret = clEnqueueReadBuffer(s_queue, mseSumBuffer, CL_TRUE, 0, sizeof(int), &result, 0, NULL, NULL);
    printf("%i\n", result);
    double finalResult = (double)result * 10.0;

    clReleaseMemObject(currentMatrixBuffer);
    clReleaseMemObject(targetMatrixBuffer);
    clReleaseMemObject(mseSumBuffer);

    return finalResult / (double)(width * height);
}

double invokeEvalKernel(Rect* rect, DMatrix* current, DMatrix* target) {

    int ret = 0;
    int width = current->cols;
    int height = current->rows;

    

    cl_mem currentMatrixBuffer = clCreateBuffer(s_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, current->cols * current->rows * sizeof(int), current->data, &ret);
    cl_mem targetMatrixBuffer = clCreateBuffer(s_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, target->cols * target->rows * sizeof(int), target->data, &ret);
    cl_mem scoreBuffer = clCreateBuffer(s_context, CL_MEM_WRITE_ONLY, sizeof(int), NULL, &ret);
    cl_mem colorBuffer = clCreateBuffer(s_context, CL_MEM_WRITE_ONLY, sizeof(int) * 3, NULL, &ret);

    int initial_result = 0;
    ret = clEnqueueWriteBuffer(s_queue, scoreBuffer, CL_TRUE, 0, sizeof(int), &initial_result, 0, NULL, NULL);

    ret = clSetKernelArg(s_kernel, 0, sizeof(int), (void*)&(rect->x));
    ret = clSetKernelArg(s_kernel, 1, sizeof(int), (void*)&(rect->y));
    ret = clSetKernelArg(s_kernel, 2, sizeof(int), (void*)&(rect->width));
    ret = clSetKernelArg(s_kernel, 3, sizeof(int), (void*)&(rect->height));
    ret = clSetKernelArg(s_kernel, 4, sizeof(cl_mem), (void*)&currentMatrixBuffer);
    ret = clSetKernelArg(s_kernel, 5, sizeof(cl_mem), (void*)&targetMatrixBuffer);
    ret = clSetKernelArg(s_kernel, 6, sizeof(cl_mem), (void*)&scoreBuffer);
    ret = clSetKernelArg(s_kernel, 7, sizeof(cl_mem), (void*)&colorBuffer);
    ret = clSetKernelArg(s_kernel, 8, sizeof(int), (void*)&width);
    ret = clSetKernelArg(s_kernel, 9, sizeof(int), (void*)&height);

    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
    double elapsedTime;
    // get ticks per second
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);


    size_t globalItemSize[] = {32, 32};
    ret = clEnqueueNDRangeKernel(s_queue, s_kernel, 2, NULL, globalItemSize, NULL, 0, NULL, NULL);
    clFinish(s_queue);

    QueryPerformanceCounter(&t2);
    // compute and print the elapsed time in millisec
    elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
    printf("Eval done in %f ms.\n", elapsedTime);


    int result;
    int color[3];
    ret = clEnqueueReadBuffer(s_queue, scoreBuffer, CL_TRUE, 0, sizeof(int), &result, 0, NULL, NULL);
    ret = clEnqueueReadBuffer(s_queue, colorBuffer, CL_TRUE, 0, sizeof(int) * 3, &color, 0, NULL, NULL);
    printf("%i\n", result);
    float finalResult = result;

    clReleaseMemObject(currentMatrixBuffer);
    clReleaseMemObject(targetMatrixBuffer);
    clReleaseMemObject(scoreBuffer);
    clReleaseMemObject(colorBuffer);

    int icolor = 0xFF000000;
    icolor += color[0] * 0x00010000;
    icolor += color[1] * 0x00000100;
    icolor += color[2] * 0x00000001;

    rect->color = icolor;

    return (double)(finalResult / (width * height));
}

void clearAllTheOpenCLStuff() {
    clReleaseKernel(s_kernel);
    clReleaseCommandQueue(s_queue);
    clReleaseProgram(s_program);
    clReleaseContext(s_context);
    clReleaseDevice(s_device);
}