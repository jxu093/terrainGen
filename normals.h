#include <vector>
#include <math.h>

std::vector<float> calcTriNormal (float t1x, float t1y, float t1z, float t2x, float t2y, float t2z) ;

std::vector<float> calcNormals (float* terr, int w, int l) ;

std::vector<float> flatNormals (float* terr, int w, int l) ;