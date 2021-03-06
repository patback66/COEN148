// Matthew Koken
// Lab 6

//  main.cpp

//  l-system

//

//  Created by yuan on 10/19/15.

//  Copyright � 2015 yuan. All rights reserved.

//

//  l-system for lab6



#include <stdlib.h>

#include <GL/GLUT.h>

#include <stdio.h>



int count = 0;

int numlevels = 0;

float view_scale = 1.f;


//F+F-F-F+F\0
char lstring[20] = "F++F--F--F++F\0";



void init(void)

{

	glClearColor(1.0, 1.0, 1.0, 0.0);

	glShadeModel(GL_FLAT);

	glPointSize(5.0);

}



void tree(int level, int leaves) {
	int current;
	current = 0;
	while (lstring[current]) {
		switch (lstring[current]) {
		case 'F':
			if (level == 0) {
				glColor3f(0.45, 0.32, 0.26);
				glBegin(GL_POLYGON);
				glVertex3f(-0.01, 0.0, 0.0);
				glVertex3f(0.01, 0.0, 0.0);
				glVertex3f(0.01, 0.1, 0.0);
				glVertex3f(-0.01, 0.1, 0.0);
				glEnd();
				glTranslatef(0.0, 0.1, 0.0);
			}
			else {
				tree(level - 1, 0);
			}
			break;
		case '[':
			glPushMatrix();
			glScalef(0.707, 0.707, 1.0);
			break;
		case ']':
			glPopMatrix();
			break;
		case '+':
			glRotatef(-60.0, 0.0, 0.0, 1.0);
			break;
		case '-':
			glRotatef(60.0, 0.0, 0.0, 1.0);
			break;
		default:
			break;
		}
		current++;
	}
}



void display(void)

{

	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.0, 0.0, 0.0);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();



	glScalef(view_scale, view_scale, 1.0);



	tree(numlevels, 1);



	glFlush();

}



void reshape(int w, int h)

{

	GLfloat asp;

	asp = (GLfloat)w / (GLfloat)h;

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	if (asp > 1.0)

		glOrtho(-2.0*asp, 2.0*asp, -0.5, 3.5, -5.0, 5.0);

	else

		glOrtho(-2.0, 2.0, -0.5 / asp, 3.5 / asp, -5.0, 5.0);

}



void mouse(int button, int state, int x, int y)

{

	switch (button) {

	case GLUT_LEFT_BUTTON:

		if (state == GLUT_DOWN) {

			numlevels++;

			glutPostRedisplay();

		}

		break;

	case GLUT_RIGHT_BUTTON:

		if ((state == GLUT_DOWN) && (numlevels > 0)) {

			numlevels--;

			glutPostRedisplay();

		}

		break;

	default:

		break;

	}

}



void keyboard(unsigned char key, int x, int y)

{

	switch (key) {

	case 27:

		exit(0);

		break;

	case 'r':

		printf("Enter new string: ");

		scanf_s("%s", lstring);

		glutPostRedisplay();

		break;

	case 'q':

		view_scale *= 2.f;

		glutPostRedisplay();

		break;

	case 'a':

		view_scale /= 2.f;

		glutPostRedisplay();

		break;

	default:

		break;

	}

}



int main(int argc, char** argv)

{

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	glutInitWindowSize(800, 800);

	glutInitWindowPosition(140, 0);

	glutCreateWindow(argv[0]);

	init();

	glutDisplayFunc(display);

	glutMouseFunc(mouse);

	glutReshapeFunc(reshape);

	glutKeyboardFunc(keyboard);

	glutMainLoop();

	return 0;

}