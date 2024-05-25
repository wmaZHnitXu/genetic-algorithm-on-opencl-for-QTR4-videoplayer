#include <stdio.h>

#include <CL/cl.h>
#include <image_presentation.h>
#include <utils.h>

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
    DMatrix* m = allocDMatrix(256, 256);
    Rect* rect = allocRect(16, 16, 64, 64, 0xFF00FFFF);

    displayMatrix(m);
    displayMatrix(m);
    
    freeDMatrix(m);    
    free(rect);
    return 0;
}