//  Matthew Koken Lab 3a
//  main.cpp
//  
//  Sample code created by:
//  Created by yuan on 10/3/15.
//  Copyright © 2015 yuan. All rights reserved.
//

#include <GL/glut.h>

class wcPt2D {
public:
	GLfloat x, y;
};

void init(void)
{

	glClearColor(1.0, 1.0, 1.0, 1.0);

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 500.0, 0.0, 500.0);
	glMatrixMode(GL_MODELVIEW);


}

void triangle(wcPt2D *verts)
{
	GLint k;

	glBegin(GL_TRIANGLES);
	for (k = 0; k<3; k++)
		glVertex2f(verts[k].x, verts[k].y);
	glEnd();

}

void displayFunc(void)
{

	// initial position for triangle
	wcPt2D verts[3] = { { 210.0,10.0 },{ 260.0,10.0 },{ 235.0,60.0 } };

	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 1.0);          // set color
	triangle(verts);                 // call own function to draw triangle


									 // draw 3 red rectangle here, and transform them


	//first rectangle: scale
	glColor3f(0.0, 0.0, 1.0);
	glLoadIdentity();
	glScalef(3, 3, 1);
	glRecti(0, 0, 100, 100);

	//second rectangle: translate
	glLoadIdentity();
	glColor3f(1.0, 0.0, 0.0);
	glTranslatef(150, 150, 0);
	glRecti(0, 0, 100, 100);

	//third rectangle: rotate
	glLoadIdentity();
	glColor3f(0.0, 1.0, 0.0);
	glTranslatef(50, 50, 0);
	glRotatef(45, 0, 0, 1);
	glTranslatef(-50, -50, 0);
	glRecti(0, 0, 100, 100);

	//base rectangle
	glLoadIdentity();
	glColor3f(0.5, 0.5, 0.5);
	glRecti(0, 0, 100, 100);
	

	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(600, 600);
	glutCreateWindow("sample program 1");
	init();
	glutDisplayFunc(displayFunc);
	glutMainLoop();
	return 0;
}
