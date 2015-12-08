/*
@author Matthew Koken <mkoken@scu.edu>
@assignment
*/
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

#include <vector>
#include <stdlib.h>

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <limits>
#include "Object.h"
#include "Vector.h"

#ifndef M_PI
#define M_PI   3.14159265358979323846264338327950288
#endif

#ifndef GL_CLAMP_TO_EDGE
const double GL_CLAMP_TO_EDGE = 0x812F;
#endif

const int width = 800, height = 800;
//Vectorf backgroundColor = Vectorf(0.52941176470588235294117647058824, 0.8078431372549019607843137254902, 0.92156862745098039215686274509804); sky blue
Vectorf backgroundColor = Vectorf(.031, .325, .659);
static const double kInfinity = std::numeric_limits<double>::max(); //define infinity as the largest double we can use, since we use doubles

const int MAX_RAY_DEPTH = 6;

std::vector<Object *> shapes;

// function to mix 2 T varables. for basic fresnel effect
template<typename T>
T mix(const T &a, const T &b, const T &mix)
{
	return b * mix + a * (T(1) - mix);
}

//calculate kr of the fresnel effect given index of refractions + 
double fresnel( const float &n1, const float &n2, const float &cosi,
  const float &cost) {
	//From wikipedia page equations
	//rs = (n1cos(theta i) - n2cos(theta t)) / (n1cos(theta i) + n2cos(theta t))
	//rp = (n2cos(theta i) - n1cos(theta t)) / (n1cos(theta t) + n2cos(theta i))
	double Rp = ((n2 * cosi) - (n1 * cost)) / ((n2 * cosi) + (n1 * cost));
	double Rs = ((n1 * cosi) - (n2 * cost)) / ((n1 * cosi) + (n2 * cost));
	return ( Rp*Rp + Rs*Rs ) * .5; //kr, kt = 1-kr
}

/*
 * The main raytrace function. Takes a ray from the given point to the next intersection with a shape or the background
 *
 */
Vectorf trace(const Ray &ray,
	const std::vector<Object *> &shapes, const int &depth) {
	// don't recurse too much. If we get too far, just give the background
	if (depth > MAX_RAY_DEPTH)
		return backgroundColor;

	double tnear = INFINITY;
	const Object *curShape = NULL;

	// Are there any shapes that the ray intersects with? We want the closest intersection
	for (int i = 0; i < shapes.size(); ++i) {
		double t0 = INFINITY, t1 = INFINITY;
		if (shapes[i]->intersect(ray, &t0, &t1)) {
			// want a hit from the outside.
			if (t0 < 0) // ray originates inside of the object, get the next hit
                t0 = t1;

			// get the closest intersecting shape, or INFINITY
			if (t0 < tnear) {
				// update the current closest intersecting object
				tnear = t0;
				curShape = shapes[i];
			}
		}
	}

	//if there is no intersecting shape, then we just have background
	if (!curShape)
        return backgroundColor;

	Vectorf surfaceColor = 0; // base color. Additive
	Vectorf pInt = ray.origin + (ray.direction * tnear);    // point of intersection.
	Vectorf nInt = curShape->getNormal(pInt);               // normal at the intersection point.

	// direction of normal must be oppostite to that of view direction
	if (ray.direction.dot(nInt) > 0)
        nInt = -nInt;

	nInt.normalize(); // normalize normal direction
					  // The angle between raydir and the normal at point hit (not used).
    float s_angle = acos(ray.direction.dot(nInt)) / ( sqrt(ray.direction.dot(ray.direction)) * sqrt(nInt.dot(nInt)));
    float s_incidence = sin(s_angle);

	double bias = 1e-2; // add some bias to the point from which we will be tracing


    // Case: Reflective and/or transparent object
	Ray refl;
	Ray refr;
	if ((curShape->transparency > 0 || curShape->reflectivity > 0) && depth < MAX_RAY_DEPTH) {
		double IdotN = ray.direction.dot(nInt);
        // I and N are not pointing in the same direction, so take the invert.
		double facingratio = std::max(double(0), -IdotN);
		// change the mix value between reflection and refraction to tweak the effect (fresnel effect)
		double fresneleffect = mix<double>(pow(1 - facingratio, 3), 1, 0.1);

		// compute reflection direction
		refl.direction = ray.direction - nInt * 2 * ray.direction.dot(nInt);
		refl.direction.normalize();
		refl.origin = pInt + (nInt * bias);

		//compute color from the reflection ray
		Vectorf reflection = trace(refl, shapes, depth + 1);
		Vectorf refraction = 0;

		//only compute refraction if transparent
		if (curShape->transparency) {
			double eta = 1 / curShape->indexOfRefraction;//double(1.2);
			double k = 1 - eta * eta * (1 - IdotN * IdotN);

			//compute refraction ray
			refr.direction = ray.direction * eta - nInt * (eta *  IdotN + sqrt(k));
			refr.origin = pInt - nInt * bias;

			//compute color from the refraction ray
			refraction = trace(refr, shapes, depth + 1);

			//refracting so can get a better fresnel effect
            float cosi = ray.direction.dot(-nInt);
            float cost = refr.direction.dot(-nInt);
			fresneleffect = fresnel(1, curShape->indexOfRefraction, cosi, cost);
		}
		// now mix the reflection and refraction parts to get the color at the pixel
		// fresnel: kt = 1 - kr, kr = fresneleffect
		surfaceColor = (reflection * fresneleffect + refraction * (1 - fresneleffect) * curShape->transparency) * curShape->color;
	}
	else {
		// it's a diffuse object, don't recurse more
		// Look at all sphere to find lights
		double shadow = 1.0;
		for (int i = 0; i < shapes.size(); i++) {
			if (shapes[i]->emissionColor.x > 0 || shapes[i]->emissionColor.y > 0 || shapes[i]->emissionColor.z > 0) {
				// this is a light, calculate the light ray
				Vectorf transmission = 1.0;
				Vectorf lightDirection = shapes[i]->center - pInt;
				lightDirection.normalize();
				//angle of the light ray
				double lightAngle = (acos(ray.direction.dot(lightDirection)) / (sqrt(ray.direction.dot(ray.direction)) * sqrt(lightDirection.dot(lightDirection))));

				//check all shapes that may be seen by the light
				for (int j = 0; j < shapes.size(); ++j) {
					if (i != j) {
						double t0, t1;

						Ray shadowRay;
						shadowRay.origin = pInt + (nInt * bias);
						shadowRay.direction = lightDirection;
						
						// check for new intersection with the light ray
						if (shapes[j]->intersect(shadowRay, &t0, &t1)) {	//if there is an intersection, then the shape is in a shadow
							shadow = std::max(0.0, shadow - (1.0 - shapes[j]->transparency));
							transmission = transmission * shapes[j]->color * shadow;
						}
						else { //not in shadow -> calculate phong
							double intensity = (0.2126*shapes[i]->emissionColor.x + 0.7152*shapes[i]->emissionColor.y + 0.0722*shapes[i]->emissionColor.z); //intensity of the visible light
							//diffuse color calculation
							surfaceColor += curShape->color * std::max(double(0), double(nInt.dot(lightDirection))) * shapes[i]->emissionColor * intensity;
							
							//specular color calculation
							Vectorf reflectionDir;
							if(shapes[j]->reflectivity>0) //need the reflection at the shape we're looking at
								reflectionDir = ray.direction - nInt * 2 * ray.direction.dot(nInt);

							//compute for phong equation once we have everything
							surfaceColor += curShape->color * pow(std::max(double(0), double(reflectionDir.dot(lightDirection))), 2) * shapes[i]->emissionColor * intensity;
						}
					}
				}
				// For each light found, we add the transmitted light
				surfaceColor += curShape->color * transmission *
					std::max(double(0), nInt.dot(lightDirection)) * shapes[i]->emissionColor;
			}
		}
	}
	return surfaceColor + curShape->emissionColor; //curShape may be a light or have some emission, don't forget to include
}



//main render loop
//calculate a ray for each pixel, raytrace and draw the point
static Vectorf cam_pos = Vectorf(0);
void render(const std::vector<Object *> &spheres)
{
	double invWidth = 1 / double(width), invHeight = 1 / double(height);
	double fov = 30, aspectratio = double(width) / double(height);
	double angle = tan(M_PI * 0.5 * fov / double(180));

	// Trace rays for all pixels on screen
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {//, ++pixel) {
            //calculate the ray's angle
			double dx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
			double dy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
			Vectorf raydir(dx, dy, -1);
			raydir.normalize();

			Ray primRay;
			primRay.direction = raydir;
			primRay.origin = cam_pos;

			Vectorf color = trace(primRay, spheres, 0);

			glPointSize(2);
			glColor3f(color.x, color.y, color.z);
			glBegin(GL_POINTS);			
			//map to the proper window coordinates, otherwise image is flipped
			glVertex2i(x, height-y);
			glEnd();
		}
	}

}


void display(void)
{
	//render the scene
	render(shapes); 
	glutSwapBuffers();

	glFlush();
}

// keyboard controls - only exit
void keyboard(unsigned char k, int x, int y) {
	switch (k)
	{
		//case '\x1b':			//getting a different key input in remote desktop
	case 27:				//escape is ascii 27
		exit(0);
		break;				// exit
	default:
		break;				//do nothing
	}

	glutPostRedisplay();
}

// gl init
void init() {
		glClearColor(1, 1, 1, 0.0);
		glMatrixMode(GL_PROJECTION);
		gluOrtho2D(0.0, width, 0.0, height);
		glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
	//initialize the shapes

	//Sphere: center, radius, surface color, type, reflectivity, transparency, index of refraction, emission color
	//Plane: point on plane, a, b, c, surface color, type, reflectivity, transparency, index of refraction, emission color
	shapes.push_back(new Plane(Vectorf(-10, -3.1, -10), 0, 1, 0, Vectorf(0.09), kDiffuse, 0, 0, 1.3));

	shapes.push_back(new Sphere(Vectorf(3, 0, -15), 2, Vectorf(1, .6, .6), kDiffuse, 1.2, 1, 5));
	shapes.push_back(new Sphere(Vectorf(1, 2, -18), 2, Vectorf(1, 1, 1), kPhong, 5, 0, 1.1));
	shapes.push_back(new Sphere(Vectorf(-2, 3, -16), 1, Vectorf(.75, 0.75, .75), kPhong, 5, 0, 1.1));
	shapes.push_back(new Sphere(Vectorf(-2, -1, -15), 2, Vectorf(0.1, 0.1, 1.0), kDiffuse, 0, 0, 2.0));
	shapes.push_back(new Sphere(Vectorf(-4, 0, -25), 1, Vectorf(1.00, 0.1, 0.1), kPhong, 0.65, 0.95, 3.0));
	shapes.push_back(new Sphere(Vectorf(5, 1.7, -25), 3.14, Vectorf(0.1, 1.0, 0.1), kDiffuse, 0, 0, 1.6));

	//shapes.push_back(new Box(Vectorf(-100, -5, -100), Vectorf(100, 5, 500), Vectorf(4, 4, 4), Vectorf(1, .2, .2), kDiffuse, 0, 0));

	// lights
	shapes.push_back(new Sphere(Vectorf(100, 300, 10), 3, Vectorf(0), kDiffuse, 0, 0, 1, Vectorf(.7))); //white light on the right
	shapes.push_back(new Sphere(Vectorf(-10, 10, 0), 3, Vectorf(0), kDiffuse, 0, 0, 1, Vectorf(.5, 0, 0))); //red light on the left

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(10, 10);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	init();

	glutMainLoop();

	//clean up
	while (!shapes.empty()) {
		Object *sph = shapes.back();
		shapes.pop_back();
		delete sph;
	}
	return 0;
}





/*
old raytrace function
bool trace(
const Ray &ray,
IsectInfo &isect,
RayType rayType = kPrimaryRay)
{
isect.hitObject = nullptr;
for (uint32_t k = 0; k < objects.size(); ++k) {
float tNear = kInfinity;
uint32_t index = 0;
Vec2f uv;
if (objects[k]->intersect(orig, dir, tNear, index, uv) && tNear < isect.tNear) {
isect.hitObject = objects[k].get();
isect.tNear = tNear;
isect.index = index;
isect.uv = uv;
}
}

return (isect.hitObject != nullptr);
}

Vectorf rayTrace(const Ray &ray,
const std::vector<Object *> &shapes, const int &depth) {
// don't recurse too much. If we get too far, just give the background
if (depth > MAX_RAY_DEPTH)
return backgroundColor;

double tnear = INFINITY;
const Object *curShape = NULL;


// Are there any shapes that the ray intersects with? We want the closest intersection
for (unsigned i = 0; i < shapes.size(); ++i) {
double t0 = INFINITY, t1 = INFINITY;
if (shapes[i]->intersect(ray, &t0, &t1)) {
// want a hit from the outside.
if (t0 < 0) // ray originates inside of the object, get the next hit
t0 = t1;

// get the closest intersecting shape, or INFINITY
if (t0 < tnear) {
// update the current closest intersecting object
tnear = t0;
curShape = shapes[i];
}
}
}

//if there is no intersecting shape, then we just have background
if (!curShape)
return backgroundColor;

Vectorf surfaceColor = 0; // base color. Additive
Vectorf pHit = ray.origin + (ray.direction * tnear);    // point of intersection.
Vectorf nHit = curShape->getNormal(pHit);               // normal at the intersection point.

// direction of normal must be oppostite to that of view direction
if (ray.direction.dot(nHit) > 0)
nHit = -nHit;

nHit.normalize(); // normalize normal direction
// The angle between raydir and the normal at point hit (not used).
float s_angle = acos(ray.direction.dot(nHit)) / (sqrt(ray.direction.dot(ray.direction)) * sqrt(nHit.dot(nHit)));
float s_incidence = sin(s_angle);

double bias = 1e-5; // add some bias to the point from which we will be tracing

//intersection exists at this point
//calculate for all lights
switch (curShape->kMatType) {
case kDiffuse: {
double shadow = 1.0;
for (int i = 0; i < shapes.size(); ++i) {
if (shapes[i]->emissionColor.x > 0 || shapes[i]->emissionColor.y > 0 || shapes[i]->emissionColor.z > 0) {
// this is a light
Vectorf transmission = 1.0;
Vectorf lightDirection = shapes[i]->center - pHit;
lightDirection.normalize();
double light_angle = (acos(ray.direction.dot(lightDirection)) / (sqrt(ray.direction.dot(ray.direction)) * sqrt(lightDirection.dot(lightDirection))));
double light_incidence = sin(light_angle);

//check all shapes that may be seen by the light
for (int j = 0; j < shapes.size(); ++j) {
if (i != j) {
double t0, t1;

Ray shadowRay;
shadowRay.origin = pHit + (nHit * bias);
shadowRay.direction = lightDirection;
// Does the ray from point hit to the light intersect an object?
// If so, calculate the shadow.
if (shapes[j]->intersect(shadowRay, &t0, &t1)) {
shadow = std::max(0.0, shadow - (1.0 - shapes[j]->transparency));
transmission = transmission * shapes[j]->color * shadow;
//break;
}
}
}
// For each light found, we add light transmission to the pixel.
surfaceColor += curShape->color * transmission *
std::max(0.0f, nHit.dot(lightDirection)) * shapes[i]->emissionColor;
}
}
break;
}
case kPhong: {
Vectorf diffuse = 0, specular = 0;
for (int i = 0; i < shapes.size(); ++i) {
if (shapes[i]->emissionColor.x > 0 || shapes[i]->emissionColor.y > 0 || shapes[i]->emissionColor.z > 0) {
Vectorf lightDir, lightIntensity;
/*struct IsectInfo
{
const Object *hitObject = nullptr;
float tNear = kInfinity;
Vec2f uv;
uint32_t index = 0;
};*/
/*
void illuminate(const Vec3f &P, Vec3f &lightDir, Vec3f &lightIntensity, float &distance) const
{
lightDir = dir;
lightIntensity = color * intensity;
distance = kInfinity;
}*/
/*lightDir = shapes[i]->center - pHit;
//calculate luminance
double intensity = (0.2126*shapes[i]->emissionColor.x + 0.7152*shapes[i]->emissionColor.y + 0.0722*shapes[i]->emissionColor.z);
lightIntensity = shapes[i]->emissionColor * intensity;

float tNear = kInfinity;

// Are there any shapes that the ray intersects with? We want the closest intersection
for (unsigned i = 0; i < shapes.size(); ++i) {
double t0 = INFINITY, t1 = INFINITY;
if (shapes[i]->intersect(ray, &t0, &t1)) {
// want a hit from the outside.
if (t0 < 0) // ray originates inside of the object, get the next hit
t0 = t1;

// get the closest intersecting shape, or INFINITY
if (t0 < tnear) {
// update the current closest intersecting object
tnear = t0;
curShape = shapes[i];
}
}
}


bool vis = !trace(hitPoint + hitNormal * options.bias, -lightDir, objects, isectShad, kShadowRay);

// compute the diffuse component
diffuse += vis * isect.hitObject->albedo * lightIntensity * std::max(0.f, hitNormal.dotProduct(-lightDir));

// compute the specular component
// what would be the ideal reflection direction for this light ray
Vec3f R = reflect(lightDir, hitNormal);
specular += vis * lightIntensity * std::pow(std::max(0.f, R.dotProduct(-dir)), isect.hitObject->n);
}
hitColor = diffuse * isect.hitObject->Kd + specular * isect.hitObject->Ks;
//std::cerr << hitColor << std::endl;
break;
}


}
return surfaceColor + curShape->emissionColor;
}*/