#include <opencl_stuff.h>

char* readKernelSource(const char* filename) {
    FILE* file = fopen(filename, "r");
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
    cl_device_id device = createDevice();
    cl_context context = createContext(&device);
    cl_program program = buildProgram(context, device);
    cl_kernel kernel = clCreateKernel(program, "kernelEntry", &err);

}