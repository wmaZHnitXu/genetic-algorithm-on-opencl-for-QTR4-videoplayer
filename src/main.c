// C standard includes
#include <stdio.h>

// OpenCL includes
#include <CL/cl.h>
#include <image_presentation.h>


int main()
{
    cl_int CL_err = CL_SUCCESS;
    cl_uint numPlatforms = 0;

    CL_err = clGetPlatformIDs( 0, NULL, &numPlatforms );

    if (CL_err == CL_SUCCESS)
        printf_s("%u platform(s) found\n", numPlatforms);
    else
        printf_s("clGetPlatformIDs(%i)\n", CL_err);

    displayImage("test.png");
    return 0;
}