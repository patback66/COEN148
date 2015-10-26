//	Matthew Koken
//  Lab 6
//  main.cpp
//  snowflake
//
//  Created by yuan on 10/24/15.
//  Copyright © 2015 yuan. All rights reserved.
//
//
//  IFS
//  Sample program for COEN290/148 Lab6

#include <stdlib.h>
#include <time.h>
#include <GL/GLUT.h>
#include <math.h>

#define WIN_WIDTH 512
#define WIN_HEIGHT 512


class wcPt2D {
public:
	GLfloat x, y;
};
wcPt2D p1, p5;
double dr = 1 / sqrt(12);

void drawSnowFlake(int d, wcPt2D p1, wcPt2D p5) {
	wcPt2D p2, p3, p4, p3p;

	if (d <= 0) {
		glBegin(GL_LINES);
		glVertex2f(p1.x, p1.y);
		glVertex2f(p5.x, p5.y);
		glEnd();
		glFlush();
	}
	else {
		double dx = p5.x - p1.x;
		double dy = p5.y - p1.y;

		p2.x = dx / 3 + p1.x;
		p2.y = dy / 3 + p1.y;
		p3.x = p2.x - dy / 3;
		p3.y = p2.y + dx / 3;
		p4.x = 2 * dx / 3 + p1.x;
		p4.y = 2 * dy / 3 + p1.y;
		p3p.x = p4.x - dy / 3;
		p3p.y = p4.y + dx / 3;
		
		drawSnowFlake(d - 1, p1, p2);
		drawSnowFlake(d - 1, p2, p3);
		drawSnowFlake(d - 1, p3, p3p);
		drawSnowFlake(d - 1, p3p, p4);
		drawSnowFlake(d - 1, p4, p5);
	}
}



void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	wcPt2D p1, p5;
	p1.x = 1;
	p1.y = 1;
	p5.x = 0;
	p5.y = 1;
	drawSnowFlake(3, p1, p5);
}

void myinit() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(0, 2, 0, 1, -1, 1);      /* Set up viewing area */
	gluOrtho2D(-0.1, 2, -0.1, 2);      /* Set up viewing area */
	glMatrixMode(GL_MODELVIEW);
	//  glPointSize(WIN_WIDTH*WIN_HEIGHT/NOS_POINTS);
	glColor3f(0.0, 0.0, 1.0);
}


int main(int argc, char **argv)
{
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	glutCreateWindow("Sierpinski Triangle");
	glutDisplayFunc(display);
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	myinit();
	glutMainLoop();
	return 0;
}