#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>
// OpenGl
#include "GL/glut.h"
#include "Object.h"
#include "Vector.h"

#ifndef M_PI
#define M_PI   3.14159265358979323846264338327950288
#endif

#ifndef GL_CLAMP_TO_EDGE
const double GL_CLAMP_TO_EDGE = 0x812F;
#endif

const int width = 800, height = 800;
GLdouble width_step = 2.0f / width;
GLdouble height_step = 2.0f / height;
Vectorf backgroundColor = Vectorf(0.3);

const int MAX_RAY_DEPTH = 3;
//const double GL_CLAMP_TO_EDGE = 0x812F;


std::vector<Sphere<double> *> spheres;

// function to mix 2 T varables.
template<typename T>
T mix(const T &a, const T &b, const T &mix)
{
	return b * mix + a * (T(1) - mix);
}

/*
 * The main raytrace function. Takes a ray from the given point to the next intersection with a shape or the background
 *
 */
template<typename T>
Vectorf trace(const Vectorf &rayorig, const Vectorf &raydir,
	const std::vector<Sphere<T> *> &spheres, const int &depth)
{
	if (depth > MAX_RAY_DEPTH)
		return backgroundColor;
	T tnear = INFINITY;
	const Object<T> *sphere = NULL;
	// Try to find intersection of this raydir with the spheres in the scene
	for (unsigned i = 0; i < spheres.size(); ++i) {
		T t0 = INFINITY, t1 = INFINITY;
		if (spheres[i]->intersect(rayorig, raydir, &t0, &t1)) {
			// is the rayorig inside the sphere (t0 < 0)?  If so, use the second hit (t0 = t1)
			if (t0 < 0) t0 = t1;
			// tnear is the last sphere intersection (or infinity).  Is t0 in front of tnear?
			if (t0 < tnear) {
				// if so, update tnear to this closer t0 and update the closest sphere
				tnear = t0;
				sphere = spheres[i];
			}
		}
	}
	// At this moment in the program, we have the closest sphere (sphere) and the closest hit position (tnear)
	// For this pixel, if there's no intersection with a sphere, return a Vec3 with the background color.
	if (!sphere) return backgroundColor; // Grey background color.
									 // if we keep on with the code, it is because we had an intersection with at least one sphere.
	Vectorf surfaceColor = 0; // initialisation of the color of the ray/surface of the object intersected by the ray.
	Vectorf phit = rayorig + (raydir * tnear); // point of intersection.
	Vectorf nhit = sphere->getNormal(phit); // normal at the intersection point.
										  // if the normal and the view direction are not opposite to each other,
										  // reverse the normal direction.
	if (raydir.dot(nhit) > 0) nhit = -nhit;
	nhit.normalize(); // normalize normal direction
					  // The angle between raydir and the normal at point hit (not used).
					  //T s_angle = acos(raydir.dot(nhit)) / ( sqrt(raydir.dot(raydir)) * sqrt(nhit.dot(nhit)));
					  //T s_incidence = sin(s_angle);
	T bias = 1e-5; // add some bias to the point from which we will be tracing
				   // Do we have transparency or reflection?
	if ((sphere->transparency > 0 || sphere->reflectivity > 0) && depth < MAX_RAY_DEPTH) {
		T IdotN = raydir.dot(nhit); // raydir.normal
									// I and N are not pointing in the same direction, so take the invert.
		T facingratio = std::max(T(0), -IdotN);
		// change the mix value between reflection and refraction to tweak the effect (fresnel effect)
		T fresneleffect = mix<T>(pow(1 - facingratio, 3), 1, 0.1);
		// compute reflection direction (not need to normalize because all vectors
		// are already normalized)
		Vectorf refldir = raydir - nhit * 2 * raydir.dot(nhit);
		Vectorf reflection = trace(phit + (nhit * bias), refldir, spheres, depth + 1);
		Vectorf refraction = 0;
		// if the sphere is also transparent compute refraction ray (transmission)
		if (sphere->transparency) {
			T ior = 1.2, eta = 1 / ior;
			T k = 1 - eta * eta * (1 - IdotN * IdotN);
			Vectorf refrdir = raydir * eta - nhit * (eta *  IdotN + sqrt(k));
			refraction = trace(phit - nhit * bias, refrdir, spheres, depth + 1);
		}
		// the result is a mix of reflection and refraction (if the sphere is transparent)
		surfaceColor = (reflection * fresneleffect + refraction * (1 - fresneleffect) * sphere->transparency) * sphere->color;
	}
	else {
		// it's a diffuse object, no need to raytrace any further
		// Look at all sphere to find lights
		double shadow = 1.0;
		for (unsigned i = 0; i < spheres.size(); ++i) {
			if (spheres[i]->emissionColor.x > 0) {
				// this is a light
				Vectorf transmission = 1.0;
				Vectorf lightDirection = spheres[i]->center - phit;
				lightDirection.normalize();
				T light_angle = (acos(raydir.dot(lightDirection)) / (sqrt(raydir.dot(raydir)) * sqrt(lightDirection.dot(lightDirection))));
				T light_incidence = sin(light_angle);
				for (unsigned j = 0; j < spheres.size(); ++j) {
					if (i != j) {
						T t0, t1;
						// Does the ray from point hit to the light intersect an object?
						// If so, calculate the shadow.
						if (spheres[j]->intersect(phit + (nhit * bias), lightDirection, &t0, &t1)) {
							shadow = std::max(0.0, shadow - (1.0 - spheres[j]->transparency));
							transmission = transmission * spheres[j]->color * shadow;
							//break;
						}
					}
				}
				// For each light found, we add light transmission to the pixel.
				surfaceColor += sphere->color * transmission *
					std::max(0.0f, nhit.dot(lightDirection)) * spheres[i]->emissionColor;
			}
		}
	}
	return surfaceColor + sphere->emissionColor;
}

// Main rendering function. We compute a camera ray for each pixel of the image,
// trace it and return a color. If the ray hits a sphere, we return the color of the
// sphere at the intersection point, else we return the background color.
Vectorf *image = new Vectorf[width * height];
float pixels[width][height][3];
static Vectorf cam_pos = Vectorf(0);
template<typename T>
void render(const std::vector<Sphere<T> *> &spheres)
{
	Vectorf *pixel = image;
	T invWidth = 1 / T(width), invHeight = 1 / T(height);
	T fov = 30, aspectratio = T(width) / T(height);
	T angle = tan(M_PI * 0.5 * fov / T(180));
	// Trace rays
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x, ++pixel) {
			T xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
			T yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
			Vectorf raydir(xx, yy, -1);
			raydir.normalize();
			Vectorf color = trace(cam_pos, raydir, spheres, 0);
			*pixel = color;
		}
	}

}

//********************************** OPEN GL ***********************************************

void advanceDisplay(void)
{
	cam_pos.z = cam_pos.z - 2;
	glutPostRedisplay();
}

void backDisplay(void)
{
	cam_pos.z = cam_pos.z + 2;
	glutPostRedisplay();
}

void resetDisplay(void)
{
	Vectorf new_cam_pos;
	new_cam_pos = cam_pos;
	cam_pos = new_cam_pos;
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			glutIdleFunc(advanceDisplay);
		}
		break;

	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_DOWN)
		{
			glutIdleFunc(resetDisplay);
		}
		break;

	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			glutIdleFunc(backDisplay);
		}
		break;
	}
}

GLuint tex = 0;
void display(void)
{

	int i;
	float x, y;
	render<double>(spheres); // Creates the image and put it to memory in image[].

	std::vector< unsigned char > buf;
	buf.reserve(width * height * 3);
	for (size_t y = 1; y <= height; ++y)
	{
		for (size_t x = 1; x <= width; ++x)
		{
			// flip vertically (height-y) because the OpenGL texture origin is in the lower-left corner
			// flip horizontally (width-x) because...the original code did so
			size_t i = (height - y) * width + (width - x);
			buf.push_back((unsigned char)(std::min(1.0f, image[i].x) * 255.0));
			buf.push_back((unsigned char)(std::min(1.0f, image[i].y) * 255.0));
			buf.push_back((unsigned char)(std::min(1.0f, image[i].z) * 255.0));
		}
	}



	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexSubImage2D
		(
			GL_TEXTURE_2D, 0,
			0, 0,
			width, height,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			&buf[0]
			);

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);
	glVertex2i(-1, -1);
	glTexCoord2i(1, 0);
	glVertex2i(1, -1);
	glTexCoord2i(1, 1);
	glVertex2i(1, 1);
	glTexCoord2i(0, 1);
	glVertex2i(-1, 1);
	glEnd();

	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	// position, radius, surface color, reflectivity, transparency, emission color
	spheres.push_back(new Sphere<double>(Vectorf(0, -10004, -20), 10000, Vectorf(0.2), 0.0, 0.0));

	spheres.push_back(new Sphere<double>(Vectorf(3, 0, -15), 2, Vectorf(1.00, 0.1, 0.1), 0.65, 0.95));
	spheres.push_back(new Sphere<double>(Vectorf(1, -1, -18), 1, Vectorf(1.0, 1.0, 1.0), 0.9, 0.9));
	spheres.push_back(new Sphere<double>(Vectorf(-2, 2, -15), 2, Vectorf(0.1, 0.1, 1.0), 0.05, 0.5));
	spheres.push_back(new Sphere<double>(Vectorf(-4, 3, -18), 1, Vectorf(0.1, 1.0, 0.1), 0.3, 0.7));

	spheres.push_back(new Sphere<double>(Vectorf(-4, 0, -25), 1, Vectorf(1.00, 0.1, 0.1), 0.65, 0.95));
	spheres.push_back(new Sphere<double>(Vectorf(-1, 1, -25), 2, Vectorf(1.0, 1.0, 1.0), 0.0, 0.0));
	spheres.push_back(new Sphere<double>(Vectorf(2, 2, -25), 1, Vectorf(0.1, 0.1, 1.0), 0.05, 0.5));
	spheres.push_back(new Sphere<double>(Vectorf(5, 3, -25), 2, Vectorf(0.1, 1.0, 0.1), 0.3, 0.7));

	// light
	spheres.push_back(new Sphere<double>(Vectorf(-10, 20, 0), 3, Vectorf(0), 0, 0, Vectorf(3)));
	spheres.push_back(new Sphere<double>(Vectorf(0, 10, 0), 3, Vectorf(0), 0, 0, Vectorf(1)));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(10, 10);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glutMainLoop();

	delete[] image;
	while (!spheres.empty()) {
		Sphere<double> *sph = spheres.back();
		spheres.pop_back();
		delete sph;
	}
	return 0;
}
