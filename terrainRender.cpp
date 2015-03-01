#include <iostream>
#include <ctime>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include "terrainHeight.h"
#include "normals.h"
// #include "materials.h"
#include "bots.h"
#include "particle.h"

float* terrain; //pointer to array for terrain
int size, sizeScale; //scaled size calculations based on terrain size
float scale; //size scale
float rotateX, rotateY; //angles of rotation
float c, c2; //variables used for index calculation
float d;
float inc2d; //for pop out window
float max, min, range; //values for terrain heights
float l0p[3], l1p[3]; //light position vectors
int meshMode; //render mode (eg. wireframe)
int lightMode; //light mode (eg. smooth shading)
int lightpos; //light position indicated by user selection
int id1, id2; //window id's

particles ps(0,50,0);

bool pause, wind, rain;
float gravity;
float speed;
float windX, windZ;


int botCount; //count for number of bots
std::vector<bot> bots; //vector of bots

bool topoMode; //topographic colouring mode on/off

std::vector<float> normals; //vector of vertex normals for smooth shading
std::vector<float> fnormals; //vector of surface normals for flat shading

void setMaterial(material m) { //function to set material
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m.amb); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m.dif); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m.spec);
	if (m.emis) glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, m.emis);
	if (m.shin) glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m.shin);
}

void setLight(lighting l) { //function to set light configuration
	glLightfv(GL_LIGHT0,GL_POSITION,l.pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l.dif);
	glLightfv(GL_LIGHT0, GL_AMBIENT, l.amb);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l.spec);
}

//PARTICLES
//PARTICLES

material randomMaterial (void) { //function to generate random color set in a "material" object defined in materials.h

	float amb[3], dif[3], spec[3];
	for (int i=0; i<3; i++) {
		amb[i]=(float(rand()%10))/10;
		dif[i]=(float(rand()%10))/10;
		spec[i]=(float(rand()%10))/10;
	}
	return {
		{amb[0],amb[1],amb[2],1},
		{dif[0],dif[1],dif[2],1},
		{spec[0],spec[1],spec[2],1}
	};
}

float randomClr (void) {
	return float(rand()%100)/100;
}

void drawParticles(void) { //draw the particles
	glPushMatrix();
	for (vIt it=ps.parts.begin(); it<ps.parts.end(); it++) { //iterate through the particle vector
		glPushMatrix();
		glTranslatef(it->pos[0],it->pos[1],it->pos[2]);
		//cout << it->pos[0] << ", " << it->pos[1] << ", " << it->pos[2] << ", age:" << it->age << "\n";
		glScalef(it->size,it->size,it->size);
		setMaterial(it->clr_mat);
		glColor3f(it->clr[0],it->clr[1],it->clr[2]);
		glRotatef(it->ang[0],1,0,0);
		glRotatef(it->ang[1],0,1,0);
		glRotatef(it->ang[2],0,0,1);
		glutSolidCube(1); //each particle is drawn as a unit cube
		glPopMatrix();
	}
	glPopMatrix();
}

void particleUpdate(void) {
	if (!pause) { //if pause is not activated
		//generate random directions, angles and sizes
		float pos[] = {rand()%size,max+5,rand()%size};
		float dir[] = {0, -1, 0};
		float ang[] = {rand() % 360, rand() % 360, rand() % 360};
		float sz = float((rand() % 30))/200;
		float clr[3];
		if (rain) {
			clr[0]=0;
			clr[1]=0;
			clr[2]=1.0;
		}
		else for (int i=0;i<3;i++) clr[i]=randomClr();
		//add to particle vector
		ps.add(pos, dir, ang, clr, randomMaterial(), speed, sz);
	}
	//update all current particles
	if (wind) ps.update(gravity, windX, windZ);
	else ps.update(gravity,0, 0);
}

//END PARTICLES
//END PARTICLES

void coloring(bool color) { //colouring code
	if (!topoMode && color) glColor3f((c-min)/range,(c-min)/range,(c-min)/range); //if topographic colouring is off, use greyscale based on height
	if (topoMode && color) { //topographic colouring on
		float gscale=(c-min)/range;
		if (gscale<0.3) {//green portion
			glColor3f(0,gscale/0.3,0);
		}
		else if (gscale>0.3 && gscale<0.35) {//yellow portion
			glColor3f((gscale-0.3)/0.05,1.0,0);
		}
		else if (gscale>0.35 && gscale<0.7) {//orange-red portion
			glColor3f(1,1-(gscale-0.35)/0.35,0);
		}
		else if (gscale>0.7) {//grey-white portion
			glColor3f((gscale-0.7)/0.3,(gscale-0.7)/0.3,(gscale-0.7)/0.3);
		}
	}
}

void tRender(float* t, int width, int length, bool color) {//terrain rendering function
	if ((lightMode+3)%3==1) {//flat shading
		for (int l=0; l<length-1; l++) { //iterate height map array
			glBegin(GL_TRIANGLE_STRIP); //draw using triangle strips

			glVertex3f(0,t[l*length],l);
			glVertex3f(0,t[(l+1)*length],l+1);

			for (int w=1; w<width; w++) {
				c = t[(l*length)+w]; //store height value so latter code is easier to read/write
				c2 = t[((l+1)*length)+w];
				coloring(color);//call colouring function
				glNormal3f(fnormals[l*(length-1)+w-1+(w-1)*3],fnormals[l*(length-1)+w+(w-1)*3],fnormals[l*(length-1)+w+1+(w-1)*3]); //set surface normal
				glVertex3f(w,c,l);
				coloring(color);
				glNormal3f(fnormals[l*(length-1)+w-1+w*3],fnormals[l*(length-1)+w+w*3],fnormals[l*(length-1)+w+1+w*3]); //set surface normal
				glVertex3f(w,c2,l+1);

			}
			glEnd();
		}
	}
	else {//no light and smooth shading
		for (int l=0; l<length-1; l++) { //iterate height map array
			glBegin(GL_TRIANGLE_STRIP); //use triangle strips to render polygons
				
			for (int w=0; w<width; w++) {
				c = t[(l*length)+w]; //store height value so latter code is easier to read/write
				c2 = t[((l+1)*length)+w];
				coloring(color);
				glNormal3f(normals[(l*length+w)*3],normals[(l*length+w)*3+1],normals[(l*length+w)*3+2]); //set vertex normal
				glVertex3f(w, c, l); 
				coloring(color);
				glNormal3f(normals[((l+1)*length+w)*3],normals[((l+1)*length+w)*3+1],normals[((l+1)*length+w)*3+2]); //set vertex normal
				glVertex3f(w, c2, l+1);
				// std::cout << w << "," << l << ",  " << w << "," << l+1 << "\n";
			}
			glEnd();
		}	

	}
}
void oneFault (float* ter, int width, int length, float d) { //single fault iteration to terrain
	float* l = randLine(width, length); //generate random line
	for (int i=0; i<width; i++) { //iterate through heights
		for (int j=0; j<length; j++) {
			if (i*l[0]+j*l[1]-l[2]>0) ter[i*width+j]+=d; //on one side of line, raise height
			else if ((ter[i*width+j]-d)>=0) ter[i*width+j]-=d; //on other side of line, lower height
		}
	}
}
void multFaults (float* ter, int w, int l) { //multiple iterations of oneFault
	for (int i=0; i<50; i++) { //first 50 iterations with increase/decrease factor of 5.0
		oneFault(ter,w,l,5.0f);
	}
	for (int i=0; i<50; i++) { //then decrease the factor every 50 iterations or so
		oneFault(ter,w,l,4.0f);
	}
	for (int i=0; i<50; i++) {
		oneFault(ter,w,l,3.0f);
	}
	for (int i=0; i<50; i++) {
		oneFault(ter,w,l,2.0f);
	}
	for (int i=0; i<100; i++) {
		oneFault(ter,w,l,1.0f);
	}
	for (int i=0; i<200; i++) {
		oneFault(ter,w,l,0.5f);
	}
}
void dualRender() { // wireframe + filled polygon mode
	glPushMatrix();
	glColor3f(0.7,0.7,0.7);
	glTranslatef(0,0.1f,0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //change to line
	tRender(terrain,size,size, 0); //render wireframe
	glPopMatrix();
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); //change to fill
}

void drawCharacter(int i) {//draw a bot from index i
	
	glPushMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// glTranslatef(slideX,terrain[slideZ*size+slideX],slideZ);
	glTranslatef(bots[i].slideX,terrain[bots[i].slideZ*size+bots[i].slideX]+2,bots[i].slideZ);

	glPushMatrix();//head
	glTranslatef(0,25,0);
	glColor3f(1,0,0);
	glutSolidSphere(5,5,5);

	glPushMatrix();//left eye
	glTranslatef(-0.5,0.5,0);
	glColor3f(0,0,0);
	glutSolidSphere(2,5,5);
	glPopMatrix();

	glPushMatrix();//right eye
	glTranslatef(0.5,0.5,0);
	glColor3f(0,0,0);
	glutSolidSphere(2,5,5);
	glPopMatrix();

	glPopMatrix();

	glPushMatrix();//body
	glTranslatef(0,15,0);
	glColor3f(0,0,1);
	glutSolidSphere(10,10,10);
	glPopMatrix();

	glPushMatrix();//leg/wheel/lower body thing
	glTranslatef(0,3,0);
	glColor3f(0.5,0,0);
	glScalef(1,1,0.1);
	glutSolidSphere(5,5,5);
	glPopMatrix();

	glPopMatrix();
}

void newBot() { //generate new bot
	bot bx = {
		rand()%(size-2*sizeScale)+sizeScale, //random x and z position coordinates
		rand()%(size-2*sizeScale)+sizeScale,
		rand()%(sizeScale), //random x and z directions
		rand()%(sizeScale)
	};
	bots.push_back(bx);

}
void moveBots() { //move the bots
	for (int i=0; i<botCount; i++) { //iterate through vector
		if (bots[i].slideX>size-(sizeScale) && bots[i].xDir>0 ) bots[i].xDir*=-1; //reverse direction if bot is reaching terrain boundary
		if (bots[i].slideX<(sizeScale) && bots[i].xDir<0 ) bots[i].xDir*=-1;
		if (bots[i].slideZ>size-(sizeScale) && bots[i].zDir>0 ) bots[i].zDir*=-1;
		if (bots[i].slideZ<(sizeScale) && bots[i].zDir<0 ) bots[i].zDir*=-1;
		bots[i].slideX+=bots[i].xDir; //change position based on direction
		bots[i].slideZ+=bots[i].zDir;
	}
}

void display() {
	if ((lightMode+3)%3==0) {//lighting turned off
		glDisable(GL_LIGHTING); //disable Lighting
		glDisable(GL_LIGHT0); //disable individual light sources
		glDisable(GL_LIGHT1);
	}
	if ((lightMode+3)%3==1 || (lightMode+3)%3==2) {//lighting turned on
		glEnable(GL_LIGHTING); //enable Lighting
		glEnable(GL_LIGHT0); //enable light sources
		glEnable(GL_LIGHT1); 
		if (lightpos==0) glLightfv(GL_LIGHT0,GL_POSITION,l0p);
		else if (lightpos==1) glLightfv(GL_LIGHT0,GL_POSITION,l0p);
		// glLightfv(GL_LIGHT1,GL_POSITION,l1p);
	}


	glClearColor(0,0,0,0); //clear background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0,20,40,0,0,0,0,1,-1); //move camera back and up looking down and forward at scene

	glRotatef(rotateX,1,0,0); //rotate scene based on user keypress
	glRotatef(rotateY,0,1,0);

	

	glScalef(scale,scale,scale); //scale scene so large terrains can be seen
	glTranslatef(-(size/2),0,-(size/2)); //shift to center 

	for (int i=0; i<botCount; i++) drawCharacter(i); //draw any existing bots


	drawParticles();

	if(((meshMode+3)%3)==0) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //normal mode
	else if (((meshMode+3)%3)==1) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
	else dualRender(); //combination mode
	tRender(terrain,size,size,1); //call terrain rendering function
	
	glutSwapBuffers();
}
void display2() { //window 2 display function for 2d height map


	glMatrixMode(GL_MODELVIEW);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glBegin(GL_POINTS); //iterate height values and draw as points
		for (int i=0; i<size; i++) {
			for (int j=0; j<size; j++) {
				d=terrain[i*size+j];

				glColor3f((d-min)/range,(d-min)/range,(d-min)/range); //colour based on height to greyscale
				glVertex2f(inc2d*i-1,inc2d*j-1);
			}
		}
	glEnd();
	glutSwapBuffers();
}
void reset() { //reset scene
	delete[] terrain; //delete terrain height values from heap memory
	terrain=randTerrain(size,size); //generate new terrain
	multFaults(terrain, size, size); //randomize terrain with faults
	scale=50.0f/size; //provide scale
	max=findMax(terrain, size, size);
	min=findMin(terrain, size, size);
	range=max-min;
	normals=calcNormals(terrain,size,size); ///calculate new normals
	fnormals=flatNormals(terrain,size,size); //calculate new flat normals
	botCount=0;//reset bot count
	bots.clear();//delete bots
}

void reshape(int w, int h) { //reshape function
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,w/h,1,200);
	glMatrixMode(GL_MODELVIEW);
}
void reshape2(int w, int h) { //reshape function for 2nd window
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// gluOrtho2D(0,200,0,200);
}

void keys(unsigned char key, int x, int y) { //keyboard
	switch (key) {
		case 27: //esc key to exit
			exit(0);
			break;
		case '1': //light position
			lightpos=0;
			break;
		case '2': //light position
			lightpos=1;
			break;
		case 'b': //add bot
			botCount++;
			newBot();
			break;
		case 'l': //change lighting
			lightMode++;
			break;
		case 'r': //reset
			reset();
			break;
		case 'w': //wireframe
			meshMode++;
			break;
		case 't': //topographic colour
			topoMode=!topoMode;
			break;
		case 43: //+ size
			scale*=1.1;
			break;
		case 45: //- size
			scale*=0.9;
			break;
	}
	glutPostWindowRedisplay(id1); //refresh both windows
	glutPostWindowRedisplay(id2);

}

void specKeys(int key, int x, int y) { //arrow keys
	switch (key) {
		case GLUT_KEY_UP:
			if (rotateX<90) rotateX+=5; //rotate about x axis
			break;
		case GLUT_KEY_DOWN:
			if (rotateX>-10) rotateX-=5;
			break;
		case GLUT_KEY_LEFT: //rotate about y axis
			rotateY+=5;
			break;
		case GLUT_KEY_RIGHT:
			rotateY-=5;
			break;
	}
	glutPostWindowRedisplay(id1);
	glutPostWindowRedisplay(id2);
}

void timer(int value) { //timer function
	moveBots(); //animate the bots
	particleUpdate();
	glutTimerFunc(45,timer,0);
	glutPostWindowRedisplay(id1);
}

void init(int argc, char** argv) {
	srand(time(0)); //set random seed

	//standard initialization calls
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(800,450);
	glutInitWindowPosition(10,10);
	id1=glutCreateWindow("3D Terrain");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keys);
	glutSpecialFunc(specKeys);
	glutTimerFunc(120, timer, 0);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST); //enable Depth


	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE); //Enable culling

	id2=glutCreateWindow("2D Heightmap"); //second window for 2d height map
	glutPositionWindow(900,10);
	glutReshapeWindow(300,300);
	glutReshapeFunc(reshape2);
	glutDisplayFunc(display2);
	glutKeyboardFunc(keys);
	glutSpecialFunc(specKeys);

}

int main(int argc, char** argv) {
	std::cout << "Enter your desired size (50-300) : ";
	std::cin >> size; //take user input size
	init(argc, argv);
	
	inc2d=2.0/size;

	terrain=randTerrain(size,size); //calculations
	multFaults(terrain, size, size);
	scale=50.0f/size;
	sizeScale=size/50;
	max=findMax(terrain, size, size);
	min=findMin(terrain, size, size);
	range=max-min;
	normals=calcNormals(terrain,size,size);
	fnormals=flatNormals(terrain,size,size);

	gravity = 0.1;
	speed = 0.1;
	rain = 1;
	// ps.setOrigin(0,max+10,0);

	l0p[0]=-(size/2); //set default light positions
	l0p[1]=max/2;
	l0p[2]=-(size/2);

	l1p[0]=(size/4);
	l1p[1]=max/2;
	l1p[2]=-(size/4);


	//the following code for text wouldn't compile if I broke it up into more than one line
	std::cout << "\n Welcome to my terrain. Here are a list of controls: \nPress arrow keys to rotate the camera about the x and y axis\nPress +/- to zoom in and out\nPress 1 or 2 to choose different light positions\nPress t to toggle topographic colouring\nPress l to toggle between no lighting(default), flat shading and smooth shading\nPress w to toggle between Normal rendering(default), wireframe rendering and a combination of both\nPress b to add a bot to the scene(can be done repeatedly)\nPress r to reset the scene\nPress esc to exit.";

	// for (int i=0;i<(size-1)*(size-1)*6;i++) std::cout << fnormals[i] << ", ";
	// printHeightMap(terrain, size, size);
	glutMainLoop();
	// return 0;
}