#include <iostream>
#include "terrainHeight.h"

float* randLine (int width, int length) { //generate random line for fault algorithm
	float v = rand();
	float a = sin(v);
	float b = cos(v);
	float d = sqrt(width*width + length*length);
	float c = float(rand()%100)/100*d - (d/2);
	float* r = new float[3];
	r[0]=a;
	r[1]=b;
	r[2]=c;
	return r;
}
// void oneFault (float* ter, int width, int length, float d) {
// 	float* l = randLine(width, length);
// 	for (int i=0; i<width; i++) {
// 		for (int j=0; j<length; j++) {
// 			if (i*l[0]+j*l[1]-l[2]>0) ter[i*width+j]+=d;
// 			else ter[i*width+j]-=d;
// 		}
// 	}
// }
float* randTerrain (int width, int length) { //generate new blank terrain with equal height of 10
	float *t = new float[width*length];
	for (int i=0; i<width; i++) {
		for (int j=0; j<length; j++) {
			t[i*width+j]=10;
		}
	}
	return t;
}
float findMax (float* t, int w, int l) { //retrieve maximum height value in a terrain
	float max=10;
	for (int i=0; i<w; i++) {
		for (int j=0; j<l; j++) {
			if (t[i*w+j]>max) max=t[i*w+j];
		}
	}
	return max;
}
float findMin (float* t, int w, int l) { //retrieve min height
	float min=10;
	for (int i=0; i<w; i++) {
		for (int j=0; j<l; j++) {
			if (t[i*w+j]<min) min=t[i*w+j];
		}
	}
	return min;
}
void printHeightMap (float* t, int w, int l) { //print height values - used for testing/debugging
	for (int i=0; i<w; i++) {
		for (int j=0; j<l; j++) {
			std::cout << t[i*w+j] << ", ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

