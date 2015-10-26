#include <gl\glut.h>

void display(void) {
	//Clear all pixels
	glClear(GL_COLOR_BUFFER_BIT);
	//draw red polygon (rectangle) with corners at
	// (0.25, 0.25, 0.0) and (0.75, 0.75, 0.0)
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_POLYGON);
	glVertex3f(0.25, 0.25, 0.0);
	glVertex3f(0.75, 0.25, 0.0);
	glVertex3f(0.75, 0.75, 0.0);
	glVertex3f(0.25, 0.75, 0.0);
	glEnd();
	// flush render objects to the screen as soon as possible
	glFlush();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	//Set Display Mode
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	//Set the window size
	glutInitWindowSize(720, 720);
	//Set the window position
	glutInitWindowPosition(100, 100);
	//Create the window
	glutCreateWindow("Lab1: Intro to OpenGL");

	//Set background to white
	glClearColor(1.0, 1.0, 1.0, 0.0);
	//initialize viewing values
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

	//Call "display" function
	glutDisplayFunc(display);
	//Enter the GLUT event loop
	glutMainLoop();
	return 0;
}