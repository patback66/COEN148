//Matthew Koken
//Lab 9

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
/*  Create checkerboard texture  */
#define checkImageWidth 64
#define checkImageHeight 64

float theta = 80.0f;
float phi = 25.0f;
float oldX, oldY;
bool rotate = false;
float zoomFactor = 1.0;
double camera_center[3] = {0.0,0.0,0.0};
float colors[13806][3];

float radians(float degrees) {
    return ( degrees * M_PI ) / 180 ;
}

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

static GLubyte checkImage[checkImageHeight][checkImageWidth][4];
static GLuint texName;
void makeCheckImage(void) {
    int i, j, c;
    for (i = 0; i < checkImageHeight; i++) {
        for (j = 0; j < checkImageWidth; j++) {
            c = ((((i&0x8)==0)^((j&0x8))==0))*255;
            checkImage[i][j][0] = (GLubyte) c;
            checkImage[i][j][1] = (GLubyte) c;
            checkImage[i][j][2] = (GLubyte) c;
            checkImage[i][j][3] = (GLubyte) 255;
        }
    }
}
void init(void){
    glClearColor (0.6, 0.6, 0.6, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    makeCheckImage();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth,
                checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, texName);

    glLoadIdentity();

        //get the radius of rotation
    float camera_radius = 3.0*zoomFactor; //want to orbit the object
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

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-100.0, 0, -100.0);
    glTexCoord2f(0.0, 10.0); glVertex3f(100.0, 0, -100.0);
    glTexCoord2f(10.0, 10.0); glVertex3f(100.0, 0, 100.0);
    glTexCoord2f(10.0, 0.0); glVertex3f(-100.0, 0, 100.0);

    glEnd();
    glFlush();
    glDisable(GL_TEXTURE_2D);
}

void reshape(int w, int h){
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, (GLfloat) w/(GLfloat) h, 0.1, 45.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //glTranslatef(0.0, 0.0, -3.6);
}

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
	default:
		break;				//do nothing
	}

	glutPostRedisplay();
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(onMouseButton);
    glutMotionFunc(OnMouseMove);
    glutMainLoop();
    return 0;
}
