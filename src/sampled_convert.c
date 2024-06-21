#include <sampled_convert.h>

DS256Stack* createSampleStack(DMatrix* original) {
    DMatrix* sample16 = allocDMatrix(16, 16);
    DMatrix* sample8 = allocDMatrix(32, 32);
    DMatrix* sample4 = allocDMatrix(64, 64);

    EMatrix* score16 = allocEMatrix(16, 16);
    EMatrix* score8 = allocEMatrix(32, 32);
    EMatrix* score4 = allocEMatrix(64, 64);

    int w = original->cols;
    int h = original->rows;
    int r = 0, g = 0, b = 0;
    int r16 = 0, r8 = 0, r4 = 0;
    int g16 = 0, g8 = 0, g4 = 0;
    int b16 = 0, b8 = 0, b4 = 0;
    
    for (int px = 0; px < h * w; px++) {
        int x = px % w;
        int y = px / w;
        int x16 = x / 16;
        int y16 = y / 16;
        int x8 = x / 32;
        int y8 = y / 32;
        int x4 = x / 64;
        int y4 = y / 64;

        char hasToClear16 = x % 16 == 0 && y % 16 == 0;
        char hasToClear8 = x % 32 == 0 && y % 32 == 0;
        char hasToClear4 = x % 64 == 0 && y % 64 == 0;
        if (hasToClear16) {
            r16 = 0; g16 = 0; b16 = 0;
        }
        if (hasToClear8) {
            r8 = 0; g8 = 0; b8 = 0;
        }
        if (hasToClear4) {
            r4 = 0; g4 = 0; b4 = 0;
        }



    }
}