#include <math.h>
#include <stdlib.h>

float* randLine (int width, int length) ;

float* randTerrain (int width, int length) ;

float findMax (float* t, int w, int l) ;
float findMin (float* t, int w, int l) ;

// void oneFault (float* ter, int width, int length) ;

void printHeightMap (float* t, int w, int l) ;