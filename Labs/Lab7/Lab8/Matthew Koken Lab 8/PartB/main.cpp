/*
 * @author Matthew Koken <mkoken@scu.edu>
 * Homework 2
 *
 * Controls:
 * Rotate model: mouse click and drag
 * Zoom: +/- buttons (-/=)
 * Move model:
 *  Up: W
 *  Down: s
 *  Left: A
 *  Right: D
 *  Forward(+Z): R
 *  Back(-Z): F
 * Reset: Backspace
 * Quit: ESC key
 -lglut -lGLU -lGL
*/

#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include "FaceData.h"

#define width 500
#define height 500

using namespace std;

float theta = 80.0f;
float phi = 25.0f;
float oldX, oldY;
bool rotate = false;
float zoomFactor = 1.0;
double camera_center[3] = {0.0,0.0,0.0};
float colors[13806][3];

int wire = 1;
int fill = 0;

float radians(float degrees) {
    return ( degrees * M_PI ) / 180 ;
}


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
    gluPerspective (90.0, 1, 0.01, 100); //50
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
		theta += (x - oldX)*0.6f;
		phi += (y - oldY)*0.6f;
	}
	//update the x,y values
	oldX = x;
	oldY = y;
	glutPostRedisplay();
}



//Draws the model based on data in FACE_DATA
void buildModel() {
    //iterate throught the list of polygons
    for(int i = 0; i < 13806; i++) {
        glBegin(GL_TRIANGLES);

        //generate a color based on which polygon we are working with
        float r = static_cast <float> ((int)(face_indicies[i][0]) * 3 % 255) / 100.00f;
        float g = static_cast <float> ((int)(face_indicies[i][1]) * 5 % 255) / 100.00f;
        float b = static_cast <float> ((int)(face_indicies[i][2]) * 7 % 255) / 100.00f;

        //glColor3f(r, g, b);
        glColor3f(0.99609375, 0.875, 0.765625);

        //n=(P2-P1) * (P3 - P1)

        //P2-P1
        GLfloat U[] = {vertices[face_indicies[i][1]][0] - vertices[face_indicies[i][0]][0],
                       vertices[face_indicies[i][1]][1] - vertices[face_indicies[i][0]][1],
                       vertices[face_indicies[i][1]][2] - vertices[face_indicies[i][0]][2]};
        //P3-P1
        GLfloat V[] = {vertices[face_indicies[i][2]][0] - vertices[face_indicies[i][0]][0],
                       vertices[face_indicies[i][2]][1] - vertices[face_indicies[i][0]][1],
                       vertices[face_indicies[i][2]][2] - vertices[face_indicies[i][0]][2]};
        //N = U x V
        GLfloat N[] = {U[1]*V[2] - U[2]*V[1], U[2]*V[0] - U[0]*V[2], U[0]*V[1] - U[1]*V[0]};

        //add the vertices for each polygon
        for(int j = 0; j < 3; j++) {
            GLfloat x = vertices[face_indicies[i][j]][0];
            GLfloat y = vertices[face_indicies[i][j]][1];
            GLfloat z = vertices[face_indicies[i][j]][2];

            //glNormal3f(normals[face_indicies[i][j+3]][0], normals[face_indicies[i][j+3]][1], normals[face_indicies[i][j+3]][2]);
            glNormal3f(N[0], N[1], N[2]);
            glVertex3f(x, y, z );
        }
        glEnd();
    }
    glEnable(GL_NORMALIZE);
}

void face_wire() {
    for(int i = 0; i < 13806; i++) {
        glBegin(GL_TRIANGLES);

        //add the vertices for each polygon
        for(int j = 0; j < 3; j++) {
            GLfloat x = vertices[face_indicies[i][j]][0];
            GLfloat y = vertices[face_indicies[i][j]][1];
            GLfloat z = vertices[face_indicies[i][j]][2];
            glVertex3f(x, y, z );
        }
        glEnd();
    }
}

void face_solid() {
    for(int i = 0; i < 13806; i++) {
        glBegin(GL_TRIANGLES);

        //add the vertices for each polygon
        for(int j = 0; j < 3; j++) {
            GLfloat x = vertices[face_indicies[i][j]][0];
            GLfloat y = vertices[face_indicies[i][j]][1];
            GLfloat z = vertices[face_indicies[i][j]][2];

            glVertex3f(x, y, z );
        }
        glEnd();
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

    //get the radius of rotation
    float camera_radius = 2.0*zoomFactor; //want to orbit the object
    float camera_pos[3] = {
        camera_center[0] + camera_radius*cos(radians(theta))*cos(radians(phi)),
        camera_center[1] + camera_radius*sin(radians(phi)),
        camera_center[2] + camera_radius*sin(radians(theta))*cos(radians(phi))
    }; //calculate the camera position based on angles of change

    gluLookAt(
        camera_pos[0], camera_pos[1], camera_pos[2],
        camera_center[0],camera_center[1],camera_center[2],
        0,1,0
    );

	//Set light position
	GLfloat light_pos[] = { 4, 2, 0, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	//Draw triangle mesh
	//glPushMatrix();
	//glScalef(zoomFactor, zoomFactor, zoomFactor);
	glDisable(GL_LIGHTING);
    if(wire && !fill) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_DEPTH_TEST);
        glColor4f(0, 0, 0, 1);
        face_wire();
    }

    if(fill && !wire) {
        glEnable(GL_LIGHTING);
        buildModel();
        glDisable(GL_LIGHTING);
    }

    if(fill && wire) {
        glEnable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor4f(0, 0, 0, 1);
        face_wire();


        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0, 1.0);
        glColor4f(1, 1, 1, 1);
        face_solid();
        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    glEnable(GL_LIGHTING);



	//glPopMatrix();


	//Flush data
	glFlush();
}

//reset the cam, etc;
void reset() {
    camera_center[0] = 0.0;
    camera_center[1] = 0.0;
    camera_center[2] = 0.0;
    theta = 80.0f;
    phi = 25.0f;
    zoomFactor = 1;
}


//keyboard input: quits on escape key pressed
void keyboard(unsigned char k, int x, int y)
{
	switch (k)
	{
	//case '\x1b':			//getting a different key input in remote desktop
	case 27:				//escape is ascii 27
		exit(0);
		break;				// exit
    case 61:
    case 107:
    //case '=':               //=/+ key
        zoomFactor -= 0.01;
        break;
    case 45:
    case 109:
    //case '-':               //-/_ key
        zoomFactor += 0.01;
        break;
    case 119:                //W
        camera_center[1]-=0.01; //model up
        break;
    case 115:                //S
        camera_center[1]+=0.01; //model down
        break;
    case 100:                //arrow right
        camera_center[0]-=0.01; //model right
        break;
    case 97:                //arrow left
        camera_center[0]+=0.01; //model left
        break;
    case 114:               //r
        camera_center[2]-=0.01;  //model forward
        break;
    case 102:               //f
        camera_center[2]+=0.01;  //model back
        break;
    case 8:                //backspace
        reset();
        break;
	default:
		break;				//do nothing
	}

	glutPostRedisplay();
}

//context menu
	//toggle between wireframe and solid rendering

//menu processor
void processMenuEvents(int option) {

	switch (option) {
	case 1:					//solid
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		wire = 0;
		fill = 1;
		break;
	case 2:					//wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		wire = 1;
		fill = 0;
		break;
    case 3:                 //wireframe hidden
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        wire = 1;
        fill = 1;
        break;
    case 4:
        glShadeModel(GL_FLAT);
        break;
    case 5:
        glShadeModel(GL_SMOOTH);
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
	glutAddMenuEntry("Wireframe - hidden", 3);
	glutAddMenuEntry("Flat Render", 4);
	glutAddMenuEntry("Render Smooth", 5);

	//set the menu to the right button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("Matthew Koken: Homework 2");

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
