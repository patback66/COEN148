/*
 *  @author Matthew Koken <mkoken@scu.edu>
 *  COEN 148 Assignment #1
 *
 *  Note: Odd behaviour on visual studio. Keep hitting right click/enter if the fill disappears
 *  Only happens on visual studio... thanks microsoft
 *
 *  Left-click to add a vertex. Right-click to close the polygon. Backspace to clear the polygon.
 *  Escape to quit.
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

#include <vector>
#include <stdlib.h>

//define standard screen/windows size. Screen size == Window Size
#define c_iScreenWidth 500
#define c_iScreenHeight 500

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


/*Globals*/
using namespace std;

void drawPoints(void);
void paintEdges(bool);
// the number of points of the polygon we've put so far
int g_iNumVertices = 0;
//is it time to fill the polygon?
bool g_bClosed = false;
//need to store the min and max for y, use extrema for initial points
int g_iYmin = c_iScreenHeight; //max of the available range is 500, limited by screenheight
int g_iYmax = 0;

//the vertices of the polygon
vector<Point> g_ptVertices;
//intersections with the horizontal - need these to know when to begin and end filling the horizontal
vector<Point> g_ptIntersections;
//For active/edge table
vector<Line> g_lnET;
//TODO: Phone home

//Init glut windows and background color
void init() {
    //white background
	glClearColor(1.0,1.0,1.0,1.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0,500.0,0.0,500.0);
	glMatrixMode(GL_MODELVIEW);
}

//drawing the points of the polygon
void paintVertices()
{
    glColor3f(1,0,0); //color them red
	glPointSize(5); //make them large enough to actually see

	//Paint a point of size (5) at each of the vertices
	for(size_t i =0; i<g_ptVertices.size(); i++) {
        glBegin(GL_POINTS);
		glVertex2i(g_ptVertices[i].x,g_ptVertices[i].y); //paint the point
		glEnd();
	}
}

//draw the edges of the polygon. If it's closed (we're done adding vertices), then the last edge is from the final vertex to the first
void paintEdges()
{
	glColor3f(0,0,1);
	//draw vertices with connecting lines
    for(size_t i = 0; i < g_ptVertices.size(); i++){
        glBegin(GL_LINES);
        glVertex2d(g_ptVertices[i].x, g_ptVertices[i].y);
        if(i+1 != g_ptVertices.size())
            glVertex2d(g_ptVertices[i+1].x, g_ptVertices[i+1].y);
        glEnd();
    }

    //close the polygon
    if(g_bClosed) {
        glBegin(GL_LINES);
        glVertex2d(g_ptVertices[g_ptVertices.size()-1].x, g_ptVertices[g_ptVertices.size()-1].y);
        glVertex2d(g_ptVertices[0].x, g_ptVertices[0].y);
        glEnd();
    }
}

//Determine the horizontal intersection pooints
bool isIntersection(Point a, Point b, int y, int&x) {
	bool withinX = false;
	bool withinY = false; //need to know if we hit an inside edge and will need to skip a concave section

    //look at the max and min of the edges
    //skip horizontal lines -> we don't count these
	if(b.y==a.y) {
		return false;
    }

    //(x2-x1)/(y2-y1) = ctg(angle between line parallel to the y axis through the point)
	x = (b.x-a.x)*(y-a.y)/(b.y-a.y) + a.x;

    //Check iif inside the y
    //Def: an insideEdgeY is inside if it is inbetween the 2 y points
	if(a.y < b.y) { //we found an inside edge at the y
		withinY = (a.y <= y) && (y <= b.y);
	} else {
		withinY = (b.y <= y) && (y <= a.y);
    }

    //Check if inside the x
    //Def: an insideEdgeX is inside if it is inbetween the 2 x points
	if(a.x < b.x) { //we found an inside edge at the x
		withinX = (a.x <= x) && (x <= b.x);
    } else {
		withinX = (b.x <= x) && (x <= a.x);
    }

    //Def: a point is INSIDE if it is both within the X and Y boundaries, color these
	return withinX && withinY;
}

//build the edge table
//stores the minimum and maximum y values of the polygon in _ymin and _ymax
void buildEdgeTable() {
    //make sure we're building a polygon
	if(g_ptVertices.size() > 2) {
	//iterate through the vertices to build the edges
		for(size_t i = 1;i<g_ptVertices.size();i++) {
            //determine mins and maxes, compare current vertex with next
			if(g_ptVertices[i-1].y < g_iYmin) {
				g_iYmin = g_ptVertices[i-1].y; //need to know ymin and ymax
            }

			if(g_ptVertices[i-1].y > g_iYmax) {
				g_iYmax = g_ptVertices[i-1].y; //need to know ymin and ymax
            }

            //make a line given the vertices, this is an edge
			Line g_iNumVertices = Line(g_ptVertices[i-1],g_ptVertices[i]);
			//add the edge to the Edge Table
			g_lnET.push_back(g_iNumVertices);
		}

		//check the min and max at the last vertex, compare last to first
		int lastVertex = g_ptVertices.size()-1;
		if(g_ptVertices[lastVertex].y > g_iYmax) {
			g_iYmax = g_ptVertices[lastVertex].y;
        }

		if(g_ptVertices[lastVertex].y < g_iYmin) {
			g_iYmin = g_ptVertices[lastVertex].y;
        }

		Line last = Line(g_ptVertices[lastVertex],g_ptVertices[0]);
		g_lnET.push_back(last);
	}
}

//sort the intersection points, gets the job done. Not efficient, but they get in the right order
void sortIntersections() {
    int intersectionCount = g_ptIntersections.size();
    for(int i = 0; i < intersectionCount-1; i++) {
        for(int j = i+1; j < intersectionCount; j++) {
            //swap 'em, move 'em down
            if(g_ptIntersections[i].x > g_ptIntersections[j].x) {
                Point tempPoint = g_ptIntersections[i];
                g_ptIntersections[i] = g_ptIntersections[j];
                g_ptIntersections[j] = tempPoint;
            }
        }
    }
}

//Fill horizontally
void horizontalFill() {
    //we've determined the intersection points, so draw horizontal lines
    //easier to draw a horizontal line than go individual pixels
    //each 'pair' is a section that needs to be filled
    for(size_t j = 1; j < g_ptIntersections.size(); j+=2) {
        glColor3f(0.8, 0.0, 1.0); //give it some color
        glBegin(GL_LINES);
        //left edge
        glVertex2i(g_ptIntersections.at(j-1).x, g_ptIntersections.at(j-1).y);
        //right edge
        glVertex2i(g_ptIntersections.at(j).x, g_ptIntersections.at(j).y);
        glEnd();
    }
}

/*
Scanline algorithm:
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

sort of using intersections in place of AET
*/

//FILL THE POLYGON, uses the above algorithm
void scanlineFill() {
    //start at the ymin, fill until the ymax
    for(int i = g_iYmin; i <= g_iYmax; i++) {
        int xIntersection;

        //accumulate the list of intersection points
        for(size_t j = 0; j < g_lnET.size(); j++) {
            //check intersections, need to keep track of intersection points given the active edge
            if (isIntersection(g_lnET[j].start, g_lnET[j].finish, i, xIntersection)) {
                Point pCur(xIntersection, i);

                //do we want to skip this one?
                if(g_lnET[j].start.y > g_lnET[j].finish.y) {
                    if(pCur.y == g_lnET[j].start.y)
                        continue; //we're done here, go to next
                } else {
                    if(g_lnET[j].start.y < g_lnET[j].finish.y) {
                        if(pCur.y == g_lnET[j].finish.y)
                            continue; //move along
                    }
                }

                g_ptIntersections.push_back(pCur); //found an intersection, store it
            }
        }

        //put the intersections in order, based on x pos.
        sortIntersections();
        //intersections are sorted, so we can color the horizontal
        horizontalFill();
        //clear intersections for the current horizontal, start fresh on the next
        g_ptIntersections.erase(g_ptIntersections.begin(),g_ptIntersections.end());
    }
}

//mouse interaction
void onMouseButton(int button, int state, int x, int y)
{
    //add a point on a left click
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !g_bClosed) //don't add vertices after closing the polygon. It does look cool though
	{
        g_iNumVertices++;
        g_ptVertices.push_back(Point(x, y));
        glutPostRedisplay();
	}

	//right click closes the polygon with whatever vertices are present
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) //TODO: DOUBLECLICK - need to spawn a timer, measure time between clicks
	{
        glFlush();
        g_bClosed = true;
        glutPostRedisplay();
	}
}

//Delete the polygon
void clearPolygon() {
    g_iNumVertices=0;
    g_bClosed=false;
    g_iYmax=0;
    g_iYmin = c_iScreenHeight;
    g_ptVertices.clear();
    g_ptIntersections.clear();
    g_lnET.clear();
}

//keyboard input: quits on escape key pressed
//Commands:
    //Escape: quit the program
    //Backspace: clear the polygon
void onKeyboard(unsigned char k, int x, int y)
{
    //enter key turns on and off the fill? what?????
	switch (k)
	{
	case 27:				//escape is ascii 27
        clearPolygon(); //clean up before exit
		exit(0);
		break;				// exit
    case 8:
        clearPolygon();
        break;
	default:
		break;				//do nothing
	}
	glutPostRedisplay();
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
	glOrtho(0,500,500,0,-1,1);
	glViewport(0,0,500,500);

    //always paint whatever vertices are present
    paintVertices();
    if(g_iNumVertices>=2) //need a polygon, minimum a triangle, so draw lines between vertices when available
        paintEdges();

	//only fill the polygon when it's done
	if(g_bClosed) {
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
    glutKeyboardFunc(onKeyboard);
    glutMouseFunc(onMouseButton);

    init();

    //main glut loop
    glutMainLoop();
    return 0;
}
