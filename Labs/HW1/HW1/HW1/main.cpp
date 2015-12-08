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

#include <vector>
#include <stdlib.h>

//define standard screen/windows size. Screen size == Window Size
#define ScreenWidth 500
#define ScreenHeight 500

/*Globals*/
using namespace std;

void drawPoints(void);
void paintEdges(bool);
// the number of points of the polygon we've put so far
int numVertices = 0;
//is it time to fill the polygon?
bool closed = false;
//need to store the min and max for y, use extrema for initial points
int point_ymin = 2000;
int point_ymax = 0;

/*Data Structures*/
//Class for a single point: ie a pixel
//Defined by an x and a y position
class Point {
public:
	float x;
	float y;
	Point(float x = 0, float y = 0) : x(x), y(y) {}
};
Point singlePoint; //generic default point

//Class for edges of a polygon
//An edge is defined by 2 points
class Line {
public:
	Point start;
	Point finish;
	Line(Point start = singlePoint, Point finish = singlePoint) : start(start), finish(finish) {}
};

//the vertices of the polygon
vector<Point> vertices;
//intersections with the horizontal - need these to know when to begin and end filling the horizontal
vector<Point> intersections;
//For active/edge table
vector<Line> ET;
//TODO: Phone home

//Determine the horizontal intersection pooints
bool findIntersection(Point a, Point b, int y, int&x) {
	bool withinX, withinY; //need to know if we hit an inside edge and will need to skip a concave section

	//look at the max and min of the edges
	//skip horizontal lines -> we don't count these
	if (b.y == a.y) {
		return false;
	}

	//(x2-x1)/(y2-y1) = ctg(angle between line parallel to the y axis through the point)
	x = (b.x - a.x)*(y - a.y) / (b.y - a.y) + a.x;

	//Def: an insideEdgeX is inside if it is inbetween the 2 x points
	if (a.x < b.x) { //we found an inside edge at the x
		withinX = (a.x <= x) && (x <= b.x);
	}
	else {
		withinX = (b.x <= x) && (x <= a.x);
	}

	//Def: an insideEdgeY is inside if it is inbetween the 2 y points
	if (a.y < b.y) { //we found an inside edge at the y
		withinY = (a.y <= y) && (y <= b.y);
	}
	else {
		withinY = (b.y <= y) && (y <= a.y);
	}

	//Def: a point is INSIDE if it is both within the X and Y boundaries, color these
	return withinX && withinY;
}

//build the edge table
void buildEdgeTable() {
	//make sure we're building a polygon
	if (vertices.size() > 2) {
		//iterate through the vertices to build the edges
		for (int i = 1; i<vertices.size(); i++) {
			//determine mins and maxes, compare current vertex with next
			if (vertices[i - 1].y < point_ymin) {
				point_ymin = vertices[i - 1].y; //need to know ymin and ymax
			}

			if (vertices[i - 1].y > point_ymax) {
				point_ymax = vertices[i - 1].y; //need to know ymin and ymax
			}

			//make a line given the vertices, this is an edge
			Line numVertices = Line(vertices[i - 1], vertices[i]);
			//add the edge to the Edge Table
			ET.push_back(numVertices);
		}

		//check the min and max at the last vertex, compare last to first
		int lastVertex = vertices.size() - 1;
		if (vertices[lastVertex].y > point_ymax) {
			point_ymax = vertices[lastVertex].y;
		}

		if (vertices[lastVertex].y < point_ymin) {
			point_ymin = vertices[lastVertex].y;
		}

		Line last = Line(vertices[lastVertex], vertices[0]);
		ET.push_back(last);
	}
}

/*
scanFill (Polygon p) {
build ET for p, intially AET is empty
scan = 0 (or lowest y-min value of ALL edges)
build AET for scan (add edges from ET whose ymin=scan)
while (active edge table is not empty) {
draw all pixels between pairs of edge intersections
++scan
update AET
1. remove any finished edges if its ymax == scan)
2. update x value of each edge in AET using (x1 = x0 + 1/m)
3. add edges (from ET) whose ymin = current scan-line
re-sort AET in the increasing order of xmin (if necessary)
}
}
*/

//FILL THE POLYGON, uses the above algorithm
void scanlineFill() {
	//start at the ymin, fill until the ymax
	for (int i = point_ymin; i <= point_ymax; i++) {
		int xIntersection;

		for (int j = 0; j < ET.size(); j++) {
			//check intersections, need to keep track of intersection points given the active edge
			if (findIntersection(ET[j].start, ET[j].finish, i, xIntersection)) {
				Point pCur(xIntersection, i);

				if (ET[j].start.y > ET[j].finish.y) {
					if (pCur.y == ET[j].start.y)
						continue; //we're done here, go to next
				}
				else {
					if (ET[j].start.y < ET[j].finish.y) {
						if (pCur.y == ET[j].finish.y)
							continue; //move along
					}
				}

				intersections.push_back(pCur); //found an intersection, store it
			}
		}

		//sort the intersection points, gets the job done. Not efficient, but they get in the right order
		int intersectionCount = intersections.size();
		Point tempPoint = Point(0, 0);
		for (int i = 0; i < intersectionCount - 1; i++) {
			for (int j = i + 1; j < intersectionCount; j++) {
				//swap 'em, move 'em down
				if (intersections[i].x > intersections[j].x) {
					tempPoint = intersections[i];
					intersections[i] = intersections[j];
					intersections[j] = tempPoint;
				}
			}
		}

		//we've determined the intersection points, so draw horizontal lines
		//easier to draw a horizontal line than go individual pixels
		//each 'pair' is a section that needs to be filled
		for (int j = 1; j < intersections.size(); j += 2) {
			glColor3f(0.8, 0.0, 1.0); //give it some color
			glBegin(GL_LINES);
			//left edge
			glVertex2i(intersections.at(j - 1).x, intersections.at(j - 1).y);
			//right edge
			glVertex2i(intersections.at(j).x, intersections.at(j).y);
			glEnd();
		}

		intersections.erase(intersections.begin(), intersections.end()); //clear intersections for the current horizontal, start fresh on the next
	}
}

//Init glut windows and background color
void initView() {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 500.0, 0.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
}

//mouse interaction
void mouse(int button, int state, int x, int y)
{
	//add a point on a left click
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !closed) //don't add vertices after closing the polygon. It does look cool though
	{
		numVertices++;
		vertices.push_back(Point(x, y));
		glutPostRedisplay();
	}
	//right click closes the polygon with whatever vertices are present
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		glFlush();
		closed = true;
		glutPostRedisplay();
	}
}

//keyboard input: quits on escape key pressed
void keyboard(unsigned char k, int x, int y)
{
	//enter key turns on and off the fill? what?????
	switch (k)
	{
	case 27:				//escape is ascii 27
		exit(0);
		break;				// exit
	default:
		break;				//do nothing
	}
	glutPostRedisplay();
}

//drawing the points of the polygon
void paintVertices()
{
	glPointSize(5);
	glBegin(GL_POINTS);
	//Paint a point of size (5) at the vertices
	for (size_t i = 0; i<vertices.size(); i++) {
		glColor3f(1, 0, 0); //color them red
		glVertex2i(vertices[i].x, vertices[i].y); //paint the point
	}
	glEnd();
}
//draw the edges of the polygon. If it's closed (we're done adding vertices), then the last edge is from the final vertex to the first
void paintEdges(bool closed)
{
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	//iterate through the vertices
	for (size_t i = 0; i < vertices.size() - 1; i++) {
		glVertex2i(vertices[i].x, vertices[i].y);
		glVertex2i(vertices[i + 1].x, vertices[i + 1].y);
	}
	if (closed) //close the polygon: vertex from last point to first point
	{
		glVertex2i(vertices[vertices.size() - 1].x, vertices[vertices.size() - 1].y); //from the last point
		glVertex2i(vertices[0].x, vertices[0].y); //to the first point
	}
	glEnd();
}

//Render the display
//If there are points selected, draw them
//If there are enough points to have at minimum 1 edge, connect all edges with a line
//If the polygon (3+ vertices) is marked as done, fill it
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//standard view
	glOrtho(0, 500, 500, 0, -1, 1);
	glViewport(0, 0, 500, 500);

	//always paint whatever vertices are present
	paintVertices();
	if (numVertices >= 2) //need a polygon, minimum a triangle, so draw lines between vertices when available
		paintEdges(closed);

	//only fill the polygon when it's done
	if (closed) {
		buildEdgeTable(); //need the edges of the polygon
		scanlineFill(); //now fill the polygon
	}
	glFlush();
}

//main
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Polygons: Left click to add a vertex, right click to finish the polygon"); //create the window

	//set callbacks
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	initView();

	//main glut loop
	glutMainLoop();
	return 0;
}
