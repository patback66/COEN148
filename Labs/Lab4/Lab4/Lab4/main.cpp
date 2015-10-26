//	Matthew Koken
//
//  main.cpp
//
//  Created by yuan on 10/10/15.
//  Copyright © 2015 yuan. All rights reserved.
//
//  sample program for lab4

#include <GL/GLUT.h>
#include <GL\GL.h>
#include <math.h>

#define PI 3.14159265359

GLint winWidth = 600, winHeight = 600;   //  Initial display-window size.

GLfloat x_0 = 150.0, y_0 = 135.0, z_0 = 40.0;  //  Viewing-coordinate origin.
GLfloat xref = 50.0, yref = 50.0, zref = -50.0;  //  Look-at point.

GLfloat Vx = 0.0, Vy = 1.0, Vz = 0.0;          //  View-up vector.

											   /*  Set coordinate limits for the clipping window:  */
GLfloat xwMin = -40.0, ywMin = -60.0, xwMax = 40.0, ywMax = 60.0;

/*  Set positions for near and far clipping planes:  */
GLfloat dnear = 25.0, dfar = 500.0;

void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glFrustum(xwMin, xwMax, ywMin, ywMax, dnear, dfar);
	gluLookAt(x_0, y_0, z_0, xref, yref, zref, Vx, Vy, Vz);

	glMatrixMode(GL_MODELVIEW);
}

void displayFcn(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glPolygonMode(GL_FRONT, GL_LINE);        //GL_LINE will display frame

	glPolygonMode(GL_BACK, GL_LINE);  //  Wire-frame back face.

									  /*  Set parameters for a square fill area.  */

	glColor3f(0.0, 1.0, 0.0);         //  Set fill color to green.

	glLoadIdentity();
	//Part 2 rotation
	glRotatef(30.0, 0.0, 1.0, 0.0);

	//Part 2: Part 2: Matrix Rotation Boogaloo
	//unit vector 0, 1, 0
	//reset
	glRotatef(-30, 0, 1, 0);
	GLfloat elems[16];
	float theta = -PI / 6;
	for (int i = 0; i<16; i++)
		elems[i] = 0.0;
	//use unit vector 0, 1, 0 so only need to apply nonzero factors to matrix
	elems[0] = cos(theta);
	elems[2] = sin(theta);
	elems[5] = 1;
	elems[8] = -sin(theta);
	elems[10] = cos(theta);
	elems[15] = 1;


	glMultMatrixf(elems);



	////Part 1: Finish the cube

	//initial face
	glBegin(GL_QUADS);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(100.0, 0.0, 0.0);
	glVertex3f(100.0, 100.0, 0.0);
	glVertex3f(0.0, 100.0, 0.0);
	glEnd();

	//face adjacent right
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glVertex3f(100.0, 0.0, 0.0);
	glVertex3f(100.0, 100.0, 0.0);
	glVertex3f(100, 100, -100);
	glVertex3f(100, 0, -100);
	glEnd();

	//face opposite
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glVertex3f(100.0, 100.0, -100.0);
	glVertex3f(0.0, 100, -100);
	glVertex3f(0.0, 0.0, -100);
	glVertex3f(100.0, 0.0, -100.0);
	glEnd();

	//face adjacent left
	glColor3f(0.5, 0.0, 0.5);
	glBegin(GL_QUADS);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 100.0, 0.0);
	glVertex3f(0.0, 100.0, -100.0);
	glVertex3f(0.0, 0.0, -100.0);
	glEnd();


	glFlush();
}

void reshapeFcn(GLint newWidth, GLint newHeight)
{
	glViewport(0, 0, newWidth, newHeight);

	winWidth = newWidth;
	winHeight = newHeight;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Perspective View of A Square");

	init();
	glutDisplayFunc(displayFcn);
	glutReshapeFunc(reshapeFcn);
	glutMainLoop();
}