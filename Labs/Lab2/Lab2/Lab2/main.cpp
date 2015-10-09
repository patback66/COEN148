/**
 *@author Matthew Koken mkoken@scu.edu
 *@Assignment COEN148 Lab 2
 */

#include <stdio.h>
#include <gl\glut.h>
#include <math.h>

float theta = 35.0f;
float phi = 60.0f;
float oldX, oldY;
float camX = 3;
float camY = 3;
float camZ = 3;
bool rotate = false;

void init(void){

	//Set background color
	glClearColor(1.0, 1.0, 1.0, 0.0);

	//Initialize lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	//Initialize camera
	glMatrixMode(GL_PROJECTION);
	gluPerspective(50, 1, 0.1, 10);
	glMatrixMode(GL_MODELVIEW);
}

//mouse drag input
//rotate view on click and drag

void onMouseButton(int button, int state, int x, int y) {
	rotate = false; //only rotate when the mouse is pressed
	if (button == GLUT_LEFT_BUTTON) {
		//store the initial mouse location
		oldX = x;
		oldY = y;
		rotate = true;
	}
}

void OnMouseMove(int x, int y) {
	if (rotate) { //only rotate while the mouse button is clicked
		//transform based on new mouse location
		theta += (x - oldX)*0.007f;
		phi += (y - oldY)*0.007f;
	}
	//update the x,y values
	oldX = x;
	oldY = y;
	glutPostRedisplay();
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

	int radius = 6; //radius of rotation
	camX = radius*cos(phi)*sin(theta);
	camY = radius*sin(phi)*sin(theta);
	camZ = radius*cos(theta);

	//assuming modelview matrix mode is set 
	gluLookAt(camX, camY, camZ, 0, 0, 0, 0, 1, 0);

	//Set light position
	GLfloat light_pos[] = { 2, 1, 0, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	//Display cube
	glColor3ub(0, 0, 255);
	glutSolidCube(1.0);

	//Flush data
	glFlush();
}

//keyboard input: quits on escape key pressed
void keyboard(unsigned char k, int x, int y)
{
	switch (k)
	{
	case '\x1b':			//getting a different key input in remote desktop
	case '27':				//escape is ascii 27
		exit(0);
		break;				// exit
	default:
		break;				//do nothing
	}

	display();
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

int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("Lab 2: User Interaction");

	//initialize interactions
	glutKeyboardFunc(keyboard);
	glutMouseFunc(onMouseButton);
	glutMotionFunc(OnMouseMove);
	createMenu();

	init();
	glutDisplayFunc(display);

	glutMainLoop();

	return 0;
}


// gluLookat(,0 0 0, 0 1 0)