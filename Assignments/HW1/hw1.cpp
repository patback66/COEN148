/**
 *@author Matthew Koken mkoken@scu.edu
 *@Assignment COEN148 Homework 1
 */

#include <stdio.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <math.h>
#include <vector>
#include <list>

int clicked = 0;
int closed = 0;
int pointStarted = 0;


/**
 * Data structures
 */
 struct GlPoint { //structure for a single point
	GLint x,y;
};

GlPoint firstpoint;
GlPoint lastpoint;


/*class GLPointArray //class for a vector of points
{
	public:
		vector<GlPoint> pt;
};*/

class Node //class for a node
{
public:
  Node():yUpper(-1),xIntersect(0.0),dxPerScan(0.0)
  { };
  int yUpper;
  float xIntersect,dxPerScan;
};
Node EmptyNode;  // an empty node

void paintpixel(int x,int y)
{
            glColor3f(1.0,1.0,1.0);
            glBegin(GL_POINTS);
            glVertex2i(x,y);
            glEnd();
}

void init(void){
	glClearColor(1.0,1.0,1.0,1.0);
	glColor3f(0.0,0.0,1.0);
	glPointSize(1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0,499.0,0.0,499.0);
}

//mouse drag input
//rotate view on click and drag

void onMouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		clicked = true;
		//add point at current location
		//start timer callback here?
		if(!pointStarted) {
			glBegin(GL_LINES);
			firstpoint.x = x;
			firstpoint.y = y;
			glVertex2i(x,y);
			pointStarted = 1;
		} else {
			lastpoint.x = x;
			lastpoint.y = y;
			glVertex2i(x,y);
			glEnd();
			glBegin(GL_LINES);
			glVertex2i(x,y);
		}
		glutPostRedisplay();
	}
	else if ((button == GLUT_LEFT_BUTTON && clicked) || button == GLUT_RIGHT_BUTTON) {
		//close the polygon
		closed = true;
		glVertex2i(firstpoint.x, firstpoint.y);
		glEnd();
		glutPostRedisplay();
	}
}

void display(void){

	//Print OpenGL errors
	GLenum err_code;
	do {
		err_code = glGetError();
		if (err_code != GL_NO_ERROR)
			printf("Error: %s\n", gluErrorString(err_code));
	} while (err_code != GL_NO_ERROR);

	//Clear buffer data
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set camera
	glLoadIdentity();

	//draw vertices with connecting lines


	//Scanline fill the polygon if it is closed
	if(closed) {
		//scanlinefill
	}


	//Flush data
	glFlush();
}

//keyboard input: quits on escape key pressed
void keyboard(unsigned char k, int x, int y)
{
	switch (k)
	{
	case 27:				//escape is ascii 27
		exit(0);
		break;				// exit
	default:
		break;				//do nothing
	}

	display();
}

int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("Lab 2: User Interaction");

	//initialize interactions
	glutMouseFunc(onMouseButton);
    glutKeyboardFunc(keyboard);
	init();
	glutDisplayFunc(display);

	glutMainLoop();

	return 0;
}
