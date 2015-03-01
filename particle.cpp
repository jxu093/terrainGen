#include "particle.h"

particles::particles(float origX, float origY, float origZ) {
	origin[0]=origX;
	origin[1]=origY;
	origin[2]=origZ;
}

void particles::setOrigin(float origX, float origY, float origZ) {
	origin[0]=origX;
	origin[1]=origY;
	origin[2]=origZ;
}

void particles::update(float g, float windX, float windZ) {
	for (vIt it = parts.begin(); it < parts.end(); it++) {

		//update position
		it->pos[0]+=it->speed*it->dir[0];
		it->pos[1]+=it->speed*it->dir[1];
		it->pos[2]+=it->speed*it->dir[2];

		// //check if particle hit platform then reverse direction (aka bounce)
		// if(it->pos[1]<=1 && it->pos[1]>=-1 && it->pos[0]>=-5 && it->pos[0]<=5 && it->pos[2]>=-5 && it->pos[2]<=5) {
		// 	if (it->dir[1]<-g) it->dir[1]*=-1;
		// 	if (friction) it->speed*=fric;
		// } 

		//update direction due to gravity
		it->dir[1]-=g;

		//update direction due to wind
		it->dir[0]+=windX;
		it->dir[2]+=windZ;

		//increment age
		it->age+=1;

		//check to kill particle 
		if (it->age>400) parts.erase(it--);



	}//end for loop

}

void particles::add(float p[3], float d[3], float a[3], float c[3], material clr_m, float s, float sz) {
	particle part;
		for (int i=0; i<3; i++) {
			part.pos[i]=p[i];
			part.dir[i]=d[i];
			part.clr[i]=c[i];
			part.ang[i]=a[i];
		}
		part.clr_mat=clr_m;
		part.speed=s;
		part.size=sz;
		part.age=0;

		parts.push_back(part);
}

