//
//  main.cpp
//  zbuffer-hidden-menu
//
//  Created by yuan on 10/26/15.
//  Copyright © 2015 yuan. All rights reserved.
//



#include <stdlib.h>
#include <GL/GLUT.h>









enum { FILL, WIRE, WIRE_HIDDEN };

GLfloat theta = 0;

int rendermode = FILL;

void menu(int selection) {
	// yuan -- change angle every time click menu
	theta = theta + 1; if (theta >= 360) theta = 0.0;
	rendermode = selection;
	glutPostRedisplay();
}

/* geometry display list names */
//enum {SPHERE = 1, CONE, WALLS};
enum { SPHERE = 1, CONE, WALLS, TEAPOT };

void  drawscene(void) {
	glCallList(WALLS);
	glPushMatrix();
	glTranslatef(-20.f, -40.f, -400.f);
	glRotatef(theta, 1, 1, 0);
	glCallList(SPHERE);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(40.f, -100.f, -420.f);

	glRotatef(-theta * 5, 1, 1, 0);

	glCallList(CONE);
	glPopMatrix();
}


/* do hidden line removal on an object in the scene */
/* use display lists to represent objects */



/* Called when window needs to be redrawn */
void redraw(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (rendermode) {
	case FILL:  /* hidden surface mode*/
		drawscene();
		break;
	case WIRE: /* basic wireframe mode */
		glDisable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor4f(0.0, 0.0, 0.0, 1.0);
		drawscene();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_LIGHTING);
		break;
	case WIRE_HIDDEN: // wireframe mode with hidden surface removal
		glDisable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST); // yuan
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor4f(0.0, 0.0, 0.0, 1.0);
		drawscene();

		// draw again with polygon offset
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		drawscene();
		glDisable(GL_POLYGON_OFFSET_FILL);


		glEnable(GL_LIGHTING);
		break;
	}
	glFlush();
}





/* ARGSUSED1 */
void key(unsigned char key, int x, int y)
{
	if (key == '\033')
		exit(0);
}

const int TEXDIM = 256;

int main(int argc, char *argv[]) {
	/* material properties for objects in scene */
	static GLfloat lightpos[] = { 50.f, 50.f, -320.f, 1.f };
	static GLfloat wall_mat[] = { 0.8f, 0.8f, 0.8f, 1.f };
	static GLfloat sphere_mat[] = { 1.0f, 0.2f, 0.2f, 1.f };
	static GLfloat cone_mat[] = { 0.2f, 1.0f, 0.2f, 1.f };
	GLUquadricObj *sphere, *cone, *base;

	glutInit(&argc, argv);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(200, 50);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	(void)glutCreateWindow("Wireframe and ZBuffer");
	glutDisplayFunc(redraw);
	glutKeyboardFunc(key);

	glutCreateMenu(menu);
	glutAddMenuEntry("Solid Fill", FILL);
	glutAddMenuEntry("Wireframe", WIRE);

	glutAddMenuEntry("Wireframe with hidden surface", WIRE_HIDDEN);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	/* draw a perspective scene */
	glMatrixMode(GL_PROJECTION);
	glFrustum(-100., 100., -100., 100., 320., 640.);
	glMatrixMode(GL_MODELVIEW);

	/* turn on features */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	/* place light 0 in the right place */
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	glNewList(SPHERE, GL_COMPILE);
	/* make display lists for sphere and cone; for efficiency */
	sphere = gluNewQuadric();
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, sphere_mat);
	gluSphere(sphere, 50.f, 40, 40);
	gluDeleteQuadric(sphere);
	glEndList();

	glNewList(CONE, GL_COMPILE);
	cone = gluNewQuadric();
	base = gluNewQuadric();
	glPushMatrix();
	glRotatef(-90.f, 1.f, 0.f, 0.f);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cone_mat);
	gluQuadricOrientation(base, GLU_INSIDE);
	gluDisk(base, 0., 40., 20, 1);
	gluCylinder(cone, 40., 0., 120., 20, 20);
	glPopMatrix();
	gluDeleteQuadric(cone);
	gluDeleteQuadric(base);
	glEndList();

	/* make display lists for walls and floor */
	glNewList(WALLS, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, wall_mat);

	/* floor */
	glBegin(GL_QUADS);
	glNormal3f(0.f, 1.f, 0.f);
	glVertex3f(-100.f, -100.f, -320.f);
	glVertex3f(100.f, -100.f, -320.f);
	glVertex3f(100.f, -100.f, -520.f);
	glVertex3f(-100.f, -100.f, -520.f);

	glBegin(GL_QUADS);

	/* left wall */
	glNormal3f(1.f, 0.f, 0.f);
	glVertex3f(-100.f, -100.f, -320.f);
	glVertex3f(-100.f, -100.f, -520.f);
	glVertex3f(-100.f, 100.f, -520.f);
	glVertex3f(-100.f, 100.f, -320.f);
	
	/* right wall */
	glNormal3f(-1.f, 0.f, 0.f);
	glVertex3f(100.f, -100.f, -320.f);
	glVertex3f(100.f, 100.f, -320.f);
	glVertex3f(100.f, 100.f, -520.f);
	glVertex3f(100.f, -100.f, -520.f);
	
	/* ceiling */
	glNormal3f(0.f, -1.f, 0.f);

	glVertex3f(-100.f, 100.f, -320.f);

	glVertex3f(-100.f, 100.f, -520.f);

	glVertex3f(100.f, 100.f, -520.f);

	glVertex3f(100.f, 100.f, -320.f);



	/* back wall */

	glNormal3f(0.f, 0.f, 1.f);

	glVertex3f(-100.f, -100.f, -520.f);

	glVertex3f(100.f, -100.f, -520.f);

	glVertex3f(100.f, 100.f, -520.f);

	glVertex3f(-100.f, 100.f, -520.f);

	glEnd();



	glEndList();



	glutMainLoop();



	return 0;

}