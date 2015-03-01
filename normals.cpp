#include "normals.h"
#include <iostream>

std::vector<float> calcTriNormal (float t1x, float t1y, float t1z, float t2x, float t2y, float t2z, float t3x, float t3y, float t3z) { 
//calculate triangle normal given 3 points
	std::vector<float> norm (3,0);

	//vector 1
	float v1x = t2x-t1x;
	float v1y = t2y-t1y;
	float v1z = t2z-t1z;
	//vector 2
	float v2x = t3x-t1x;
	float v2y = t3y-t1y;
	float v2z = t3z-t1z;

	norm[0]=v1y*v2z-v1z*v2y;
	norm[1]=v1z*v2x-v1x*v2z;
	norm[2]=v1x*v2y-v1y*v2x;

	return norm;
}
float vLength(std::vector<float> vec) { //calculate length/magnitude of a vector
	return sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
}

std::vector<float> calcNormals (float* terr, int w, int l) { //calculate vertex normals of terrain array
	std::vector<float> norms (w*l*3,0);

	for (int z=0; z<l; z++) {
		for (int x=0; x<w; x++) {
			std::vector<float> vecs (3,0);

			if (z>0 && x>0) { 
				std::vector<float> n;
				n = calcTriNormal(x,terr[z*l+x],z,x,terr[(z-1)*l+x],z-1,x-1,terr[z*l+x-1],z);
				float l=vLength(n);
				vecs[0]+=n[0]/l;
				vecs[1]+=n[1]/l;
				vecs[2]+=n[2]/l;
			}
			if (x>0 && z<l-1) {
				std::vector<float> n;
				n = calcTriNormal(x,terr[z*l+x],z,x-1,terr[z*l+x-1],z,x,terr[(z+1)*l+x],z+1);
				float l=vLength(n);
				vecs[0]+=n[0]/l;
				vecs[1]+=n[1]/l;
				vecs[2]+=n[2]/l;
			}
			if (z<l-1 && x<w-1) {
				std::vector<float> n;
				n = calcTriNormal(x,terr[z*l+x],z,x,terr[(z+1)*l+x],z+1,x+1,terr[z*l+x+1],z);
				float l=vLength(n);
				vecs[0]+=n[0]/l;
				vecs[1]+=n[1]/l;
				vecs[2]+=n[2]/l;
			}
			if (x<w-1 && z>0) {
				std::vector<float> n;
				n = calcTriNormal(x,terr[z*l+x],z,x+1,terr[z*l+x+1],z,x,terr[(z-1)*l+x],z-1);
				float l=vLength(n);
				vecs[0]+=n[0]/l;
				vecs[1]+=n[1]/l;
				vecs[2]+=n[2]/l;
			}
			float vl=vLength(vecs);
			norms[(z*l+x)*3]=vecs[0]/vl;
			norms[(z*l+x)*3+1]=vecs[1]/vl;
			norms[(z*l+x)*3+2]=vecs[2]/vl;


		}
	}

	return norms;
}

std::vector<float> flatNormals (float* terr, int w, int l) { //calculate surface normals for flat shading
	std::vector<float> norms ((w-1)*(l-1)*6,0);
	int c=0;
	for (int z=0;z<l-1;z++) {
		for (int x=0;x<w-1;x++) {
			std::vector<float> n;
			n=calcTriNormal(x,terr[z*l+x],z,x+1,terr[z*l+x+1],z,x,terr[(z+1)*l+x],z+1);
			float ln = vLength(n);
			int p1=z*(l-1)*6+6*x;
			int p2=z*(l-1)*6+6*x+1;
			int p3=z*(l-1)*6+6*x+2;
			norms[p1]=n[0]/ln;
			norms[p2]=n[1]/ln;
			norms[p3]=n[2]/ln;
			// std::cout<< "x: " << x << ", z: " << z << ", " << p1 << ", " <<p2 << ", " << p3 << ", calc: " << l-1 << "\n";

			c+=3;

		}
		for (int x2=1;x2<w;x2++) {
			std::vector<float> n;
			n=calcTriNormal(x2,terr[(z+1)*l+x2],z+1,x2,terr[z*l+x2],z,x2-1,terr[(z+1)*l+x2-1],z+1);
			float ln = vLength(n);
			norms[z*(l-1)*6+6*x2-3]=n[0]/ln;
			norms[z*(l-1)*6+6*x2-2]=n[1]/ln;
			norms[z*(l-1)*6+6*x2-1]=n[2]/ln;
			// std::cout<<norms[z*(l-1)+x2-1+3*x2] << ", " << norms[z*(l-1)+x2+3*x2] << ", " << norms[z*(l-1)+x2+1+3*x2] << "\n";
			c+=3;

		}
	}

	// std::cout<< "FINAL COUNT:" << c << " / " << (w-1)*(l-1)*6 << "\n";
	// for (int i=0; i<(w-1)*(l-1)*6; i++) std::cout << norms[i] << ", ";

	return norms;
}