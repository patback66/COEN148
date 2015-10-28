//
//  main.cpp
//
//  Created by yuan on 10/10/15.
//  Copyright © 2015 yuan. All rights reserved.
//
//  sample program for lab4

#include <GL/GLUT.h>

GLint winWidth = 600, winHeight = 600;   //  Initial display-window size.

GLfloat x_0 = 75.0, y_0 = 65.0, z_0 = 85.0;  //  Viewing-coordinate origin.
GLfloat xref = 0.0, yref = 0.0, zref = 0.0;  //  Look-at point.

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

//context menu
//toggle between wireframe and solid rendering

//menu processor
void processMenuEvents(int option) {

	switch (option) {
	case 1:					//solid
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 2:					//wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	default:
		break;
	}

	glutPostRedisplay();		//update the window
}

//Initialize the menu
void createMenu() {

	int menu;
	menu = glutCreateMenu(processMenuEvents);

	//Menu options
	glutAddMenuEntry("Solid", 1);
	glutAddMenuEntry("Wireframe", 2);

	//set the menu to the right button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void displayFcn(void)
{
	glClear(GL_COLOR_BUFFER_BIT);


									  /*  Set parameters for a square fill area.  */

	//glLoadIdentity();

	//sphere
	glPushMatrix();
	glColor3f(0.0, 1.0, 0.0);
	glTranslatef(60, 60, -100);
	glutSolidSphere(30, 30, 30);
	glPopMatrix();

	//cone
	glPushMatrix();
	glRotatef(0, 25, 0, 0);
	glTranslatef(-450, 50, -100);
	glColor3f(0.0, 0.0, 1.0);
	glutSolidCone(60, 70, 40, 40);
	glPopMatrix();

	//cylinder
	GLUquadricObj *my_cyl;
	my_cyl = gluNewQuadric();
	gluQuadricDrawStyle(my_cyl, GLU_FILL);
	glPushMatrix();
	glColor3f(0.5, 0.5, 1);
	
	glTranslatef(95, -40, -10);
	glRotatef(90, 12, 0, 0);
	gluCylinder(my_cyl, 50, 50, 50, 30, 30);
	glPopMatrix();
	

	//ellipsoid
	glPushMatrix();
	glTranslatef(-250, 200, -80);
	glColor3f(1.0, 0.5, 1.0);
	glScalef(3, 0.25, 0.50);
	glutSolidSphere(60, 30, 30);
	glPopMatrix();

	//other shape
	glPushMatrix();
	glTranslatef(-300, 250, -10);
	glColor3f(0.5, 1, 0.2);
	glutSolidTeapot(30);
	glPopMatrix();

	//TRIANGLES FOR THE CUBE
	glPushMatrix();
	
	//back face first
	glBegin(GL_TRIANGLES);
	glColor3f(0.1, 1, 0.5);
	glVertex3f(0, 0, 0);
	glVertex3f(50, 0, 0);
	glVertex3f(0, 50, 0);
	glEnd();	
	
	glBegin(GL_TRIANGLES);
	glColor3f(1, 0.1, 0.5);
	glVertex3f(0, 50, 0);
	glVertex3f(50, 50, 0);
	glVertex3f(50, 0, 0);
	glEnd();

	//now bottom
	glBegin(GL_TRIANGLES);
	glColor3f(50, 50, 0.5);
	glVertex3f(0, 0, 0);
	glVertex3f(50, 0, 0);
	glVertex3f(50, 0, 50);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(0.1, 0, 0.5);
	glVertex3f(0, 0, 0);
	glVertex3f(50, 0, 50);
	glVertex3f(0, 0, 50);
	glEnd();

	//back adjacent
	glBegin(GL_TRIANGLES);
	glColor3f(1, 0, 0.5);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50, 0);
	glVertex3f(0, 50, 50);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(0, 1, 3);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50, 50);
	glVertex3f(0, 0, 50);
	glEnd();

	//front
	glBegin(GL_TRIANGLES);
	glColor3f(0, 0, 3);
	glVertex3f(0, 0, 50);
	glVertex3f(0, 50, 50);
	glVertex3f(50, 50, 50);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(3, 0, 3);
	glVertex3f(50, 50, 50);
	glVertex3f(50, 0, 50);
	glVertex3f(0, 0, 50);
	glEnd();

	//front adjacent
	glBegin(GL_TRIANGLES);
	glColor3f(1, 0.3, 0);
	glVertex3f(50, 0, 50);
	glVertex3f(50, 50, 50);
	glVertex3f(50, 50, 0);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(.5, 1, 0.5);
	glVertex3f(50, 50, 0);
	glVertex3f(50, 0, 50);
	glVertex3f(50, 0, 0);
	glEnd();

	//top
	glBegin(GL_TRIANGLES);
	glColor3f(0.1, 3, 0);
	glVertex3f(0, 50, 0);
	glVertex3f(50, 50, 0);
	glVertex3f(50, 50, 50);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(.5, .1, 5);
	glVertex3f(50, 50, 50);
	glVertex3f(0, 50, 50);
	glVertex3f(0, 50, 0);
	glEnd();
	glPopMatrix();



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
	glutCreateWindow("Perspective View of Objects");
	createMenu();

	init();
	glutDisplayFunc(displayFcn);
	glutReshapeFunc(reshapeFcn);
	glutMainLoop();
}