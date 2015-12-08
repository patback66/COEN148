//Matthew Koken
//Homework 3, elements from Lab 9
#include <stdio.h>
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

#ifndef M_PI
#define M_PI   3.14159265358979323846264338327950288
#endif

#include <algorithm>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include "Vector.h"
#include "Object.h"

//using namespace std;
/*  Create checkerboard texture  */
#define checkImageWidth 64
#define checkImageHeight 64

#define screenWidth 600
#define screenHeight 600
#define max_recur 6
#ifndef INFINITY
#define INFINITY 1e8
#endif
#define FOV 45

float theta = 80.0f;
float phi = 25.0f;
float oldX, oldY;
bool rotate = false;
float zoomFactor = 1.0;
double camera_center[3] = { 0.0,0.0,0.0 };
float aspectRatio = screenWidth / (float)screenHeight;

const Vectorf backgroundColor = Vectorf(0.5f, 0.62f, 0.78f);

std::vector<Object*> allShapes;
std::vector<Object*> allLights;

float radians(float degrees) {
	return (degrees * M_PI) / 180;
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

float reflectAndRefract(float a, float b, float mix)
{
	return b * mix + a * (1 - mix);
}

void initScene() {
	/*center, radius, surface color, refl, refr, transp, ec, type*/
	// position, radius, surface color, reflectivity, transparency, emission color

	//center, radius, color, n, refl, tran, isLight
	/*allShapes.push_back(new Sphere(Vectorf(0, 0, 15), 3, Vectorf(.5, .7, .5), 1.3, 1, 1.3));


	allShapes.push_back(new Sphere(Vectorf(-4.5, 4.5, 17), 1.8, Vectorf(1, .3, .3), 0.7, 1, 1.3));
	allShapes.push_back(new Sphere(Vectorf(2, -2, 19), 2, Vectorf(.3, 1, .3), 0.5, 0.5, 0.9));
	//spheres.push_back(new Sphere<double>(Vectorf(3, 0, -15), 2, Vectorf(1.00, 0.1, 0.1), 0.65, 0.95));
	allShapes.push_back(new Sphere(Vectorf(3, 0, -15), 2, Vectorf(1, 0.1, 0.1), 0, 0, 0.65));

	allShapes.push_back(new Sphere(Vectorf(3, -3, -13), .5, Vectorf(0, 1, 0), 1, 1, 1, true));


	//light
	allLights.push_back(new Sphere(Vectorf(3, -3, 13), .5, Vectorf(50), 0, 0, 1, true));*/
	allShapes.push_back(new Sphere(Vectorf(0, -10004, -20), 10000, Vectorf(0.2), 1, 0.0, 0.0));

	/*allShapes.push_back(new Sphere(Vectorf(3, 0, -15), 2, Vectorf(1.00, 0.1, 0.1), 1.3, 0.65, 0.95));
	allShapes.push_back(new Sphere(Vectorf(1, -1, -18), 1, Vectorf(1.0, 1.0, 1.0), 1.3, 0.9, 0.9));
	allShapes.push_back(new Sphere(Vectorf(-2, 2, -15), 2, Vectorf(0.1, 0.1, 1.0), 1.3, 0.05, 0.5));
	allShapes.push_back(new Sphere(Vectorf(-4, 3, -18), 1, Vectorf(0.1, 1.0, 0.1), 1.3, 0.3, 0.7));

	allShapes.push_back(new Sphere(Vectorf(-4, 0, -25), 1, Vectorf(1.00, 0.1, 0.1), 1.3, 0.65, 0.95));
	allShapes.push_back(new Sphere(Vectorf(-1, 1, -25), 2, Vectorf(1.0, 1.0, 1.0), 1.3, 0.0, 0.0));
	allShapes.push_back(new Sphere(Vectorf(2, 2, -25), 1, Vectorf(0.1, 0.1, 1.0), 1.3, 0.05, 0.5));
	allShapes.push_back(new Sphere(Vectorf(5, 3, -25), 2, Vectorf(0.1, 1.0, 0.1), 1.3, 0.3, 0.7));

	// light
	allShapes.push_back(new Sphere(Vectorf(-10, 20, 0), 3, Vectorf(3), 1, 0, 0, true));
	allShapes.push_back(new Sphere(Vectorf(0, 10, 0), 3, Vectorf(1), 1, 0, 0, true));*/

	allShapes.push_back(new Sphere(Vectorf(3, 0, -15), 2, Vectorf(1, 0.1, 0.1), .5, 1.2, 1.3));

	allShapes.push_back(new Sphere(Vectorf(-10, 20, 0), 3, Vectorf(3), 0, 0, 0, true));
	allShapes.push_back(new Sphere(Vectorf(0, 10, 0), 3, Vectorf(1), 0, 0, 0, true));
}

/*Vectorf rayTrace(Vectorf ray, int depth, Vectorf color) {

}
raytrace(ray, recursive-depth, color ) {
if(recursive-depth > TRACEDEPTH) return 0;
// ﬁnd the nearest intersection
for( i = 0; i< all-objects; i++) {
//  intersection calculation
//  note, the intersection could be a hit from within the object in the case of

// refraction
}
// determine color at intersection
//  if you want to display light source, you can set light sources as part of the scene
//  objects,  then when for intersection, you will just simply set a white color to that
//  point as light color.

for (all objects) {
if (it is light source) {
//set light color
}
else {

// use illumination model to calculate color (diffuse and specular) at the
//  intersection
}
}
// calculate reﬂection
// recursively call raytrace()
raytrace(reﬂection-ray, recursive-depth+1, col);
color += col;
// calculate refraction
// recursively call  raytrace()
raytrace(refraction-ray, recursive-depth+1, col);
color += col;
}*/


void init(void) {
	glClearColor(1, 1, 1, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 600.0, 0.0, 600.0);
	glMatrixMode(GL_MODELVIEW);

	initScene();
}

void computeSnell(Vectorf incidentRay, Vectorf surfaceNormal, float n1, float n2, Vectorf *reflectionDir, Vectorf *refractionDir) {
	float ration = n1 / n2;
	float r1 = incidentRay.dot(surfaceNormal);
	float r2 = sqrt(1 - (ration * ration) * (1 - r1 * r1));
	*reflectionDir = incidentRay - surfaceNormal * (2 * r1);
	*refractionDir = incidentRay * ration + surfaceNormal * (r2 - ration * r1);
}

float computeFresnelEffect(float etai, float etat, float cosi, float cost) {
	float Rp = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
	float Rs = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
	return (Rp * Rp + Rs * Rs) * 0.5f;
}

template<typename T>
T mix(const T &a, const T &b, const T &mix)
{
	return b * mix + a * (T(1) - mix);
}



//gives the direction of the new ray from a reflection
Ray computeReflectionRay(const Ray &ray, Vectorf nHit, Vectorf pHit, float bias) {
	Ray refl;
	refl.origin = pHit + nHit * bias;
	refl.direction = ray.direction - nHit * 2 * ray.direction.dot(nHit);
	return refl;
}

Ray computeRefractionRay(float IdotN, const Ray &ray, Vectorf nHit, Vectorf pHit, float bias) {
	float ior = 1.2, eta = 1 / ior;
	float k = 1 - eta * eta * (1 - IdotN * IdotN);
	Ray refr;
	refr.direction = ray.direction * eta - nHit * (eta *  IdotN + sqrt(k));
	refr.origin = pHit - nHit * bias;
	return refr;
}

float computeDistance(Vectorf a, Vectorf b) {
	float xd = b.x - a.x;
	float yd = b.y - a.y;
	float zd = b.z - a.z;
	return sqrt(xd*xd + yd*yd + zd*zd);

}

Vectorf trace(const Ray &ray, int depth) {
	//RECURSIVE CHECK FIRST
	//hit the max, so we're at background at this point
	/*if (depth > max_recur) {
		return backgroundColor;
	}*/

	Object* shape = NULL;
	float minDist = INFINITY;
	Vectorf pHit; //point of intersection
	Vectorf nHit; //normal to the intersection
	bool inside = false;
	float bias = 1e-4;
	float distance;
	float t0 = INFINITY;
	float t1 = INFINITY;

	//calculate the intersections between the ray and the next object, we want the closest shape with an intersection
	for (int i = 0; i < allShapes.size(); i++) {
		if (allShapes[i]->intersect(ray, &t0, &t1)) {
			if (t0 < minDist) {
				if (t0 < 0) //ray originates from inside the sphere
					t0 = t1; //so use the second intersection

				//If the intersection is in front of the last sphere (or infinity, the background) we use it
				if (t0 < minDist) {
					minDist = t0;
					shape = allShapes[i];
				}
			}
		}
	}

	//no shape, get background
	if (!shape)
		return backgroundColor;
									 // if we keep on with the code, it is because we had an intersection with at least one sphere.
	Vectorf curColor = 0; // initialisation of the color of the ray/surface of the object intersected by the ray.
	Vectorf phit = ray.origin + (ray.direction * minDist); // point of intersection.
	Vectorf nhit = phit - shape->center; // normal at the intersection point.
										  // if the normal and the view direction are not opposite to each other,
										  // reverse the normal direction.
	if (ray.direction.dot(nhit) > 0)
		nhit = -nhit;

	nhit.normalize(); // normalize normal direction
					  // The angle between raydir and the normal at point hit (not used).
					  //T s_angle = acos(raydir.dot(nhit)) / ( sqrt(raydir.dot(raydir)) * sqrt(nhit.dot(nhit)));
					  //T s_incidence = sin(s_angle);

	if ((shape->transparency > 0 || shape->reflectivity > 0) && depth < max_recur) {
		float IdotN = ray.direction.dot(nhit); // raydir.normal
									// I and N are not pointing in the same direction, so take the invert.
		float facingratio = std::max(0.0f, -IdotN);
		// change the mix value between reflection and refraction to tweak the effect (fresnel effect)
		float fresneleffect = mix<float>(pow(1 - facingratio, 3), 1, 0.1);
		// compute reflection direction (not need to normalize because all vectors
		// are already normalized)
		Ray refl;
		refl.direction = ray.direction - nhit * 2 * ray.direction.dot(nhit);
		refl.origin = phit + (nhit * bias);
		Vectorf reflection = trace(refl, depth + 1);

		Vectorf refraction = 0;
		// if the sphere is also transparent compute refraction ray (transmission)
		if (shape->transparency) {
			float ior = 1.2, eta = 1 / ior;
			float k = 1 - eta * eta * (1 - IdotN * IdotN);
			Ray refr;
			refr.direction = ray.direction * eta - nhit * (eta *  IdotN + sqrt(k));
			refr.origin = phit - nhit * bias;
			refraction = trace(refr, depth + 1);
		}
		// the result is a mix of reflection and refraction (if the sphere is transparent)
		curColor = (reflection * fresneleffect + refraction * (1 - fresneleffect) * shape->transparency) * shape->color;
	}
	else {
		// it's a diffuse object, no need to raytrace any further
		// Look at all sphere to find lights
		float shadow = 1.0;
		for (unsigned i = 0; i < allShapes.size(); ++i) {
			if (allShapes[i]->isLight) {
				// this is a light
				Vectorf transmission = 1.0;
				Vectorf lightDirection = allShapes[i]->center - phit;
				lightDirection.normalize();
				float light_angle = (acos(ray.direction.dot(lightDirection)) / (sqrt(ray.direction.dot(ray.direction)) * sqrt(lightDirection.dot(lightDirection))));
				float light_incidence = sin(light_angle);
				for (unsigned j = 0; j < allShapes.size(); ++j) {
					if (i != j) {
						// Does the ray from point hit to the light intersect an object?
						// If so, calculate the shadow.
						Ray shadowRay;
						shadowRay.origin = phit + (nhit * bias);
						shadowRay.direction = lightDirection;
						if (allShapes[j]->intersect(shadowRay, &t0, &t1)) {
							shadow = std::max(0.0f, shadow - (1.0f - allShapes[j]->transparency));
							transmission = transmission * allShapes[j]->color * shadow;
							//break;
						}
					}
				}
				// For each light found, we add light transmission to the pixel.
				curColor += shape->color * transmission *
					std::max(0.0f, nhit.dot(lightDirection)) * allShapes[i]->color;
			}
		}
	}
	return curColor + shape->color;
}


void computeBaseRay(int i, int j, Ray *ray) {
	float frameAspectRatio = screenWidth / (float)screenHeight;
	float angle = tan(FOV * 0.5f * M_PI / 180.0f);
	ray->origin = Vectorf(0);
	float dx = 2 * frameAspectRatio / (float)screenWidth;
	float dy = 2 / (float)screenHeight;
	ray->direction.x = angle * ((i + 0.5) * dx - frameAspectRatio);
	ray->direction.y = angle * (-(j + 0.5) * dy + 1);
	ray->direction.z = 1;
	ray->direction.normalize();
}

float pixels[600][600][3];
void render()
{
	/*std::cout << "about to make pixels" << std::endl;

	float invWidth = 1 / float(screenWidth);
	float invHeight = 1 / float(screenHeight);
	float aspectratio = screenWidth / float(screenHeight);
	float angle = tan(M_PI * 0.5 * FOV / 180.);

	for (int x = 0; x < screenHeight; x++) {
		for (int y = 0; y < screenWidth; y++) {
			Ray primRay;
			computeBaseRay(x, y, &primRay);


			//Vectorf color =
			Vectorf color = trace(primRay, 0);
			pixels[x][y][0] = color.x;
			pixels[x][y][1] = color.y;
			pixels[x][y][2] = color.z;

			//normalize
			if (pixels[x][y][0] > 1.f) pixels[x][y][0] = 1.f;
			if (pixels[x][y][1] > 1.f) pixels[x][y][1] = 1.f;
			if (pixels[x][y][2] > 1.f) pixels[x][y][2] = 1.f;

			//std::cout << "R: " << color.x << " G: " << color.y << " B: " << color.z << std::endl;
			/*glPointSize(1);
			glBegin(GL_POINTS);
			glColor3f(color.x, color.y, color.z);
			glVertex2i(x, y);
			glEnd();
		}
	}
	glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, pixels);*/

	float invWidth = 1 / (float)screenWidth, invHeight = 1 / (float)screenHeight;
	float aspectratio = (float)screenWidth / (float)screenHeight;
	float angle = tan(M_PI * 0.5 * FOV / 180.0f);
	// Trace rays
	for (GLuint y = 0; y < screenHeight; ++y) {
		for (GLuint x = 0; x < screenWidth; ++x) {
			float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
			float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
			Ray primRay;
			primRay.direction = Vectorf(xx, yy, -1);
			primRay.origin = Vectorf(0, 0, 0);
			primRay.direction.normalize();
			Vectorf color = trace(primRay, 0);

	//		std::cout << color.x << " " << color.y << " " << color.z << std::endl;
			pixels[x][y][0] = color.x;
			pixels[x][y][1] = color.y;
			pixels[x][y][2] = color.z;
						//normalize
			if (pixels[x][y][0] > 1.f) pixels[x][y][0] = 1.f;
			if (pixels[x][y][1] > 1.f) pixels[x][y][1] = 1.f;
			if (pixels[x][y][2] > 1.f) pixels[x][y][2] = 1.f;

		}
	}
	glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, pixels);
	std::cout << "Done" << std::endl;
}


void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//standard view
	glOrtho(0, 600, 600, 0, -1, 1);
	glViewport(0, 0, 600, 600);

	/*glLoadIdentity();

	//get the radius of rotation
	float camera_radius = 3.0*zoomFactor; //want to orbit the object
	float camera_pos[3] = {
	camera_center[0] + camera_radius*cos(radians(theta))*cos(radians(phi)),
	camera_center[1] + camera_radius*sin(radians(phi)),
	camera_center[2] + camera_radius*sin(radians(theta))*cos(radians(phi))
	}; //calculate the camera position based on angles of change
	*/

	/*gluLookAt(
	camera_pos[0], camera_pos[1], camera_pos[2],
	camera_center[0],camera_center[1],camera_center[2],
	0,1,0
	);*/
	std::cout << "ABout to render" << std::endl;
	render();
	glFlush();
	//glDisable(GL_TEXTURE_2D);
}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, (GLfloat)w / (GLfloat)h, 0.1, 4500.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(0.0, 0.0, -3.6);
}

void keyboard(unsigned char k, int x, int y) {
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

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
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
