#include <GL/glut.h>
#include <vector>

struct Point
{
    Point( float x, float y ) : x(x), y(y) {}
    float x, y;
};
std::vector< Point > points;

void mouse( int button, int state, int x, int y )
{
    if( button == GLUT_LEFT_BUTTON && state == GLUT_UP )
    {
        points.push_back( Point( x, y ) );
    }
    if( button == GLUT_RIGHT_BUTTON && state == GLUT_UP )
    {
        points.clear();
    }
    glutPostRedisplay();
}

void display()
{
    glClearColor( 0, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    int w = glutGet( GLUT_WINDOW_WIDTH );
    int h = glutGet( GLUT_WINDOW_HEIGHT );
    glOrtho( 0, w, h, 0, -1, 1 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glColor3ub( 255, 0, 0 );
    glBegin( GL_QUADS );
    for( size_t i = 0; i < points.size(); ++i )
    {
        const unsigned int SIZE = 20;
        const Point& pt = points[ i ];
        glVertex2i( pt.x - SIZE, pt.y - SIZE );
        glVertex2i( pt.x + SIZE, pt.y - SIZE );
        glVertex2i( pt.x + SIZE, pt.y + SIZE );
        glVertex2i( pt.x - SIZE, pt.y + SIZE );
    }
    glEnd();

    glutSwapBuffers();
}

int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
    glutInitWindowSize( 640, 480 );
    glutCreateWindow( "GLUT" );
    glutMouseFunc( mouse );
    glutDisplayFunc( display );
    glutMainLoop();
    return 0;
}
