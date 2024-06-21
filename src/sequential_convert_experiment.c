#include <sequential_convert_experiment.h>

void mutateAllRects(Node* prevGen, DMatrix* clearCurrent, DMatrix* target) {
    int count = countList(prevGen);
    for (int i = 0; i < count; i++) {
        mutateCoveredRect(i, prevGen, clearCurrent, target);
    }
}

void mutateCoveredRect(int rectId, Node* generation, DMatrix* clearCurrent, DMatrix* target) {
    DMatrix* current = renderCurrentBefore(rectId, generation, clearCurrent);

    int childrenCount = 1000;

    Rect *rect = allocRect(0, 0, 0, 0, 0);
    Node* prevRectNode = generation;
    for (int i = 0; i < rectId; i++) {
        prevRectNode = prevRectNode->next;
    }
    Rect* prevRect = prevRectNode->rect;
    Rect* best = prevRect;
    
    int** coverage = getCoverageMatrix(rectId, generation, clearCurrent);
    double d = evalCoveredRectOnMatrix(prevRect, current, target, coverage);
    prevRect->score = d;
    double oldscore = d;
    
    for (int j = 0; j < childrenCount; j++) {
        //MUTATION
        rect->x = prevRect->x + rand8() - 4;
        rect->y = prevRect->y + rand8() - 4;
        if (rect->x > 255) rect->x = 255;
        if (rect->y > 255) rect->y = 255;
        if (rect->x < 0) rect->x = 0;
        if (rect->y < 0) rect->y = 0;

        rect->width = prevRect->width + rand8() - 4;
        rect->height = prevRect->height + rand8() - 4;
        if (rect->x + rect->width > 256) rect->width = 256 - rect->x;
        if (rect->y + rect->height > 256) rect->height = 256 - rect->y;
        if (rect->width < 1) rect->width = 1;
        if (rect->height < 1) rect->height = 1;

        rect->color = getCoveredAvgColor(rect, target, coverage);
        rect->score = evalCoveredRectOnMatrix(rect, current, target, coverage);
        //
        if (rect->score > best->score) {
            //free(best);
            best = allocCopyOfRect(rect);
        }
    }
    prevRectNode->rect = best;

    for (int i = 0; i < clearCurrent->rows; i++) {
            free(coverage[i]);
    }
    free(coverage);

    freeDMatrix(current);

    printf("%i Score delta: %f  dx:%i dy:%i dw:%i dh:%i\n", rectId,  best->score - prevRect->score,
        prevRect->x - best->x, prevRect->y - best->y, prevRect->width - best->width, prevRect->height - best->height);
    /*
    QueryPerformanceCounter(&t2);
    // compute and print the elapsed time in millisec
    elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
    printf("Mutation done in %f ms.\n", elapsedTime);
    */   
}

DMatrix* renderCurrentBefore(int rectId, Node* generation, DMatrix* current) {
    DMatrix* rendered = allocDMatrix(current->cols, current->rows);
    for (int i = 0; i < current->cols * current->rows; i++) {
        rendered->data[i] = current->data[i];
    }

    Node* currentNode = generation;
    for (int i = 0; i < rectId - 1; i++) {
        drawRectOnDMatrix(currentNode->rect, rendered);
        currentNode = currentNode->next;
    }

    return rendered;
}

DMatrix* renderCurrentSubverted(int rectId, Rect* subversion, Node* generation, DMatrix* current) {
    DMatrix* rendered = allocDMatrix(current->cols, current->rows);

    Node* currentNode = generation;
    for (int i = 0; currentNode != NULL; i++) {
        drawRectOnDMatrix(i != rectId ? currentNode->rect : subversion, rendered);
        currentNode = currentNode->next;
    }

    return rendered;
}

int getCoveredAvgColor(Rect *rect, DMatrix *target, int** coverageMatrix) {
    int w = target->cols;
    int h = target->rows;

    int x_start = rect->x;
    int x_end = rect->x + rect->width;
    x_end = x_end < w ? x_end : w;

    int y_start = rect->y;
    int y_end = rect->y + rect->height;
    y_end = y_end < h ? y_end : h;

    int channelSumms[4];
    for (int k = 0; k < 4; k++) channelSumms[k] = 0;
    unsigned char* test = NULL;
    int pxCount = 0;

    int width = target->cols;
    for (int i = y_start; i < y_end; i++) {
        for (int j = x_start; j < x_end; j++) {
            if (coverageMatrix[i][j] != 0) {
                continue;
            }
            unsigned char* colors = &(target->data[i * width + j]);
            for (int k = 0; k < 4; k++) {
                channelSumms[k] += colors[k];
            }
            //printf("color:0x%08x\n", target->data[i][j]);
            test = colors;
            pxCount++;
        }
    }

    //0xFFFFFFFF
    //   3 2 1 0
    if (pxCount == 0) {
        return 0x00000000;
    }
    int r = channelSumms[2] / pxCount;
    int g = channelSumms[1] / pxCount;
    int b = channelSumms[0] / pxCount;
    //printf("r%ig%ib%i %f\n", channelSumms[2], channelSumms[1], channelSumms[0], g);

    int result = 0xFF000000;
    result += r * 0x00010000;
    result += g * 0x00000100;
    result += b * 0x00000001;
    return result;
}

double evalCoveredRectOnMatrix(Rect* rect, DMatrix* current, DMatrix* target, int** coverage) {
    float oldSum = 0.0;
    float newSum = 0.0;

    int w = target->cols;
    int h = target->rows;

    int x_start = rect->x;
    int x_end = rect->x + rect->width;
    x_end = x_end < w ? x_end : w;

    int y_start = rect->y;
    int y_end = rect->y + rect->height;
    y_end = y_end < h ? y_end : h;

    unsigned char* color = (unsigned char*)&rect->color;

    int width = target->cols;
    int px = 0;
    for (int i = y_start; i < y_end; i++) {
        //printf("testastat111");
        for (int j = x_start; j < x_end; j++) {
            unsigned char* targetColor = &(target->data[i * width + j]);
            unsigned char* currentColor = &(current->data[i * width + j]);

            if (coverage[i][j] != 0) {
                currentColor = (unsigned char*)&coverage[i][j];
            }

            int diff[4];
            int odiff[4];
            for (int k = 0; k < 4; k++) {
                diff[k] = (coverage[i][j] == 0 ? color[k] : currentColor[k]) - targetColor[k];
                diff[k] = diff[k] * diff[k];

                odiff[k] = currentColor[k] - targetColor[k];
                odiff[k] = odiff[k] * odiff[k];
            }
            float err = diff[0] + diff[1] + diff[2] + diff[3];
            float oerr = odiff[0] + odiff[1] + odiff[2] + odiff[3];

            newSum += err;
            oldSum += oerr;
            px++;
            //printf("testastat1112");
        }
        
    }
    //if (px > 0)
    //printf("testastat1112 %i %f %f\n", px, newSum, oldSum);
    float divider = current->cols * current->rows;
    float mse = (oldSum - newSum) / divider;
    return (double)mse;
}

int** getCoverageMatrix(int coveredRectId, Node* population, DMatrix* currentMatrix) {
    int width = currentMatrix->cols;
    int height = currentMatrix->rows;

    // Allocate the coverage matrix
    int** coverageMatrix = malloc(sizeof(int*) * height);
    for (int i = 0; i < height; i++) {
        coverageMatrix[i] = malloc(sizeof(int) * width);
        for (int j = 0; j < width; j++) {
            coverageMatrix[i][j] = 0;
        }
    }

    Node* current = population;
    for (int i = 0; i <= coveredRectId && current != NULL; i++) {
        current = current->next;
    }

    while (current != NULL) {
        Rect* rect = current->rect;
        int x_start = rect->x;
        int x_end = rect->x + rect->width;
        int y_start = rect->y;
        int y_end = rect->y + rect->height;

        // Ensure the coordinates are within the matrix boundaries
        if (x_end > width) x_end = width;
        if (y_end > height) y_end = height;

        for (int i = y_start; i < y_end; i++) {
            for (int j = x_start; j < x_end; j++) {
                coverageMatrix[i][j] = rect->color;
            }
        }
        current = current->next;
    }

    return coverageMatrix;
}
