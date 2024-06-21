#include <sequential_convert.h>

void mutateAllRects(Node* prevGen, DMatrix* clearCurrent, DMatrix* target);
int** getCoverageMatrix(int coveredRectId, Node* population, DMatrix* currentMatrix);
double evalCoveredRectOnMatrix(Rect* rect, DMatrix* current, DMatrix* target, char** coverage);
DMatrix* renderCurrentBefore(int rectId, Node* generation, DMatrix* current);
void mutateCoveredRect(int rectId, Node* generation, DMatrix* clearCurrent, DMatrix* target);
DMatrix* renderCurrentSubverted(int rectId, Rect* subversion, Node* generation, DMatrix* current);
int getCoveredAvgColor(Rect *rect, DMatrix *target, int** coverageMatrix);