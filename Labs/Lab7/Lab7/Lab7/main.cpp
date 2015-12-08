//  Matthew Koken
//  Lab7
//  main.cpp
//  cube
//
//  Created by yuan on 10/31/15.
//  Copyright © 2015 yuan. All rights reserved.
//
//  For coen290/148, Lab7, Fall 2015


#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>


GLfloat light_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };  /* Red diffuse light. */
GLfloat light_position[] = { 1, 1, 1, 2 };  /* Infinite light location. */
GLfloat n[6][3] = {  /* Normals for the 6 faces of a cube. */
	{ -1.0, 0.0, 0.0 },{ 0.0, 1.0, 0.0 },{ 1.0, 0.0, 0.0 },
	{ 0.0, -1.0, 0.0 },{ 0.0, 0.0, 1.0 },{ 0.0, 0.0, -1.0 } };

GLint faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
	{ 0, 1, 2, 3 },{ 3, 2, 6, 7 },{ 7, 6, 5, 4 },
	{ 4, 5, 1, 0 },{ 5, 6, 2, 1 },{ 7, 4, 0, 3 } };

GLfloat v[8][3];  /* Will be filled in with X,Y,Z vertexes. */

GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_ambient[] = { 0.7, 1.7, 0.7, 1.0 };
GLfloat mat_ambient_color[] = { 0.8, 0.8, 0.2, 1.0 };
GLfloat mat_diffuse[] = { 0.2, 0.4, 0.9, .9 };
GLfloat mat_specular[] = { 1.5, 1.4, 1.3, 1.2 };
GLfloat no_shininess[] = { 0.0 };
GLfloat low_shininess[] = { 3.0 };
GLfloat high_shininess[] = { 88.0 };
GLfloat mat_emission[] = { 0.2, 0.1, 0.4, 0.0 };

void drawBox(void) {
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
	int i;
	for (i = 0; i < 6; i++) {
		glBegin(GL_QUADS);
		glNormal3fv(&n[i][0]);
		glVertex3fv(&v[faces[i][0]][0]);
		glVertex3fv(&v[faces[i][1]][0]);
		glVertex3fv(&v[faces[i][2]][0]);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}

void drawSphere() {
	glPushMatrix();

	glTranslatef(1.25, 3.0, 0.0);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
	glutSolidSphere(1.0, 50, 50);
	glPopMatrix();
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glLoadIdentity();
	drawSphere();

	/* Adjust cube position to be asthetic angle. */
	glPushMatrix();
	glTranslatef(0.0, 0.0, -1.0);
	glRotatef(60, 1.0, 0.0, 0.0);
	glRotatef(-20, 0.0, 0.0, 1.0);
	drawBox();
	glPopMatrix();
	glutSwapBuffers();
}


void init(void) {
	/* Setup cube vertex data. */
	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = 1;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = -1;

	/* Enable a single OpenGL light. */
	//local light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	

	/*Second Light*/
	//spot light
	GLfloat cutoff[] = { 27.0 };
	GLfloat spot_direction[] = { -5.0, 0, 0.0 };
	GLfloat spot_position[] = { 4, 0, 0, 0 };
	GLfloat spot_diffuse[] = { 0.3, 0.8, 1, 1};
	glLightfv(GL_LIGHT1, GL_POSITION, spot_position);
	glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, cutoff);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, spot_diffuse);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);
	glEnable(GL_LIGHT1);

	//global ambient light
	GLfloat global_ambient[] = { 1, 0, 0, 0 };
	GLfloat global_position[] = { -3, 3, -3, 0 };
	glLightfv(GL_LIGHT2, GL_DIFFUSE, global_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, global_position);
	glEnable(GL_LIGHT2);

	glEnable(GL_LIGHTING);


	/* Use depth buffering for hidden surface elimination. */
	glEnable(GL_DEPTH_TEST);

	/* Setup the view of the cube. */
	glMatrixMode(GL_PROJECTION);
	gluPerspective( /* field of view in degree */ 40.0,
		/* aspect ratio */ 1.0,
		/* Z near */ 1.0, /* Z far */ 100.0);

	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0.0, 0.0, 12.0,  /* eye is at (0,0,5) */
		0.0, 0.0, 0.0,      /* center is at (0,0,0) */
		0.0, 1.0, 0.);      /* up is in positive Y direction */
	glMatrixMode(GL_MODELVIEW);
}



int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("red 3D lighted cube");
	glutDisplayFunc(display);
	init();
	glutMainLoop();
	return 0;
}
