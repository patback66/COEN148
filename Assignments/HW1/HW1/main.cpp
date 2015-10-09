/**
 *@author Matthew Koken mkoken@scu.edu
 *@Assignment COEN148 Homework 1
 */

#include <stdio.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <vector>
using namespace std;


int clicked = 0;
int closed = 0;
int pointStarted = 0;


/**
 * Data structures
 */
struct Point
{
    Point( float x, float y ) : x(x), y(y) {}
    float x, y;
};
//Holds the vertices for the polygon
std::vector< Point > vertices;
std::vector< Point > interserctions;

//Color the specified point
void paintPixel(int x,int y)
{
    glColor3f(1.0,0.0,0.0); //paint a different color than the edges of the polygon
    glBegin(GL_POINTS);
    glVertex2i(x,y); //line with only 1 point is 1 pixel
    glEnd();
}

//Init the windows, etc. for opengl
void init(void){
	glClearColor(1.0,1.0,1.0,1.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0,500.0,0.0,500.0);
	glMatrixMode(GL_MODELVIEW);
}

//mouse interactions
//Left click adds a vertex to the polygon
//Right click closes the polygon with whatever vertices are present
//TODO: Double click to close
void onMouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && !closed) {
		clicked = true;
		//add point at current location
		//start timer callback here for?
        vertices.push_back(Point(x, 500-y)); //account for window height
        glutPostRedisplay();
	}
	//right click closes for now
	else if ((button == GLUT_LEFT_BUTTON && clicked) || button == GLUT_RIGHT_BUTTON) {
		//close the polygon
		closed = true;
		glutPostRedisplay();
	}
}

//determine the edge between 2 vertices- we want the minimums and maximums to determine
//where we will paint
void EdgeScan(Point a, Point b, int *lep, int *rep)
{
    float x,mx;
    Point temp(0,0);

    //Case: If the y of point a is greater than the y of point b, neg slope edge
    //set a to be the min
    if(a.y>b.y)
    {
        //store a so we can swap the points
        temp.x = a.x;
        temp.y = a.y;

        //swap a and b
        a.x = b.x;
        a.y = b.y;
        b.x = temp.x;
        b.y = temp.y;
    }

    //Case: horizontal line
    if(a.y == b.y)
        mx=b.x-a.x; //get the length
    //Case: rest of the shape
    else
        mx=(b.x-a.x)/(b.y-a.y);

    x=a.x; //set the new x to look at

    //set to color horizontally from left edge until the right edge for each x/y max and min
    for(int i = int(a.y); i <= (int)b.y; i++)
    {
        if(x<(float)lep[i])
            lep[i]=(int)x;
        if(x>(float)rep[i])
            rep[i]=(int)x;
        x+=mx;
    }
}

//scan all of the vertex pairs to determine where to fill
void scanlineFill(){
    //buckets for the pixels, fit the size of the window
    int leftEdge[500],rightEdge[500];

    for(int i=0;i<500;i++) {//set initial values
        leftEdge[i]=500;
        rightEdge[i]=0;
    }

    //scan each vertex - line pair
    for(size_t t = 0; t < vertices.size()-1; t++) {
        EdgeScan(vertices[t], vertices[t+1], leftEdge, rightEdge);
    }

    //scan the last vertex-line pair
    EdgeScan(vertices[vertices.size()-1], vertices[0], leftEdge, rightEdge);

    //now we know the boundaries, so paint horizontally
    for(int j=0;j<500;j++)
    {
        if(leftEdge[j]<=rightEdge[j]) {
            for(int i=leftEdge[j];i<rightEdge[j];i++) {
                paintPixel(i,j);
            }
        }
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

	glColor3f(0.0,0.0,1.0);

	//Clear buffer data
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set camera
	glLoadIdentity();

	//draw vertices with connecting lines
    for(size_t i = 0; i < vertices.size(); i++){
        glBegin(GL_LINES);
        glVertex2d(vertices[i].x, vertices[i].y);
        if(i+1 != vertices.size())
            glVertex2d(vertices[i+1].x, vertices[i+1].y);
        glEnd();
    }

    //close the polygon
    if(closed) {
        glBegin(GL_LINES);
        glVertex2d(vertices[vertices.size()-1].x, vertices[vertices.size()-1].y);
        glVertex2d(vertices[0].x, vertices[0].y);
        glEnd();
    }

	//Scanline fill the polygon if it is closed
	if(closed) {
		//polygon is complete, we can fill it now
		scanlineFill();
	}

	//Flush data
	glFlush();
}

//keyboard input: quits on escape key pressed
void keyboard(unsigned char k, int x, int y)
{
	switch (k)
	{
	case 27:				//escape is ascii 27
		exit(0);
		break;				// exit
	default:
		break;				//do nothing
	}

	display();
}

int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("Homework 1: Left click to add a vertex, double/right click to close");

	//initialize interactions
	glutMouseFunc(onMouseButton);
    glutKeyboardFunc(keyboard);

	init();
	glutDisplayFunc(display);

	glutMainLoop();

	return 0;
}

//glutTimerFunc
