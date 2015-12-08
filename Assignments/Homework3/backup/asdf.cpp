//Matthew Koken
//Homework 3, elements from Lab 9

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
#include <stdio.h>
#include <iostream>
/*  Create checkerboard texture  */
#define checkImageWidth 64
#define checkImageHeight 64

using namespace std;

float theta = 80.0f;
float phi = 25.0f;
float oldX, oldY;
bool rotate = false;
float zoomFactor = 1.0;
double camera_center[3] = {0.0,0.0,0.0};

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


/*
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

rayTrace(ray)

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

//draw the chessboard
void drawCheckerBoard() {
    glBegin(GL_QUADS);
    //iterative drawing
    //i -> x coordinates
    //j -> z coordinates
    //draw board on x,z plane -> y always 0
	for(int i = -100; i < 100; i+=5) {
		for(int j = -100; j < 100; j+=5) {

            //oscillate the colors
			if((abs(i)+abs(j))%2) {
                glColor3f(1, 1, 1);
            }
            else {
                glColor3f(0, 0, 0);
            }

            //add the vertices for the current square
            glVertex3f(i/10.0f, 0, j/10.0f);
			glVertex3f((i+5)/10.0f, 0, j/10.0f);
			glVertex3f((i+5)/10.0f, 0, (j+5)/10.0f);
			glVertex3f(i/10.0f, 0, (j+5)/10.0f);
		}
	}
	glEnd();
}

void drawEllipse() {
	glPushMatrix();
	glTranslatef(-.95, 1.0, .89);
	glColor3f(1.0, 0.5, 1.0);
	glScalef(1.23, 0.65, 0.50);
	glutSolidSphere(.60, 30, 30);
	glPopMatrix();
}

void drawSphere() {
    glPushMatrix();
    glColor3f(0.1f, 0.1f, 0.5f);
    glTranslatef(.75, 1.0, 0.0);
    glutSolidSphere(.50, 50, 50);
    glPopMatrix();
}

void addLight() {
    /* Enable a single OpenGL light. */
	//local light
	GLfloat light_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };  /* Red diffuse light. */
    GLfloat light_position[] = { -2, 1, -2, 2 };  /* Infinite light location. */
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    //glBindTexture(GL_TEXTURE_2D, texName);

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



    drawCheckerBoard();
    drawSphere();
    drawEllipse();

    glFlush();
    //glDisable(GL_TEXTURE_2D);
}

void reshape(int w, int h){
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, (GLfloat) w/(GLfloat) h, 0.1, 4500.0);
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

/*
Vectorf trace(Vectorf rayorig, Vectorf raydir, int depth)
{
    //if (raydir.length() != 1) std::cerr << "Error " << raydir << std::endl;
    float tnear = INFINITY;
    const Shape* shape = NULL;
    // find intersection of this ray with the sphere in the scene
    for (int i = 0; i < allShapes.size(); ++i) {
        float t0 = INFINITY, t1 = INFINITY;
        //if (allShapes[i].intersect(rayorig, raydir, t0, t1)) {
            if (t0 < 0) t0 = t1;
            if (t0 < tnear) {
                tnear = t0;
                shape = &allShapes[i];
            }
       // }
    }
    // if there's no intersection return black or background color
    if (!shape) return Vectorf(2);
    Vectorf surfaceColor = 0; // color of the ray/surfaceof the object intersected by the ray
    Vectorf phit = rayorig + raydir * tnear; // point of intersection
    Vectorf nhit = phit - shape->center; // normal at the intersection point
    nhit.normalize(); // normalize normal direction
    // If the normal and the view direction are not opposite to each other
    // reverse the normal direction. That also means we are inside the sphere so set
    // the inside bool to true. Finally reverse the sign of IdotN which we want
    // positive.
    float bias = 1e-4; // add some bias to the point from which we will be tracing
    bool inside = false;
    if (raydir.dot(nhit) > 0) nhit = -nhit, inside = true;
    if ((shape->transparency > 0 || shape->reflection > 0) && depth < max_recur) {
        float facingratio = -raydir.dot(nhit);
        // change the mix value to tweak the effect
        float fresneleffect = reflectAndRefract(pow(1 - facingratio, 3), 1, 0.1);
        // compute reflection direction (not need to normalize because all vectors
        // are already normalized)
        Vectorf refldir = raydir - nhit * 2 * raydir.dot(nhit);
        refldir.normalize();
        Vectorf reflection = trace(phit + nhit * bias, refldir, depth + 1);
        Vectorf refraction = 0;
        // if the sphere is also transparent compute refraction ray (transmission)
        if (shape->transparency) {
            float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
            float cosi = -nhit.dot(raydir);
            float k = 1 - eta * eta * (1 - cosi * cosi);
            Vectorf refrdir = raydir * eta + nhit * (eta *  cosi - sqrt(k));
            refrdir.normalize();
            refraction = trace(phit - nhit * bias, refrdir, depth + 1);
        }
        // the result is a mix of reflection and refraction (if the sphere is transparent)
        surfaceColor = (
            reflection * fresneleffect +
            refraction * (1 - fresneleffect) * shape->transparency) * shape->surfaceColor;
    }
    else {
        // it's a diffuse object, no need to raytrace any further
        for (unsigned i = 0; i < allShapes.size(); ++i) {
            if (allShapes[i].emissionColor.x > 0) {
                // this is a light
                Vectorf transmission = 1;
                Vectorf lightDirection = allShapes[i].center - phit;
                lightDirection.normalize();
                for (unsigned j = 0; j < allShapes.size(); ++j) {
                    if (i != j) {
                        float t0, t1;
                        if (allShapes[j].intersect(phit + nhit * bias, lightDirection, t0, t1)) {
                            transmission = 0;
                            break;
                        }
                    }
                }
                surfaceColor += shape->surfaceColor * transmission *
                std::max(float(0), nhit.dot(lightDirection)) * allShapes[i].emissionColor;
            }
        }
    }

    return surfaceColor + shape->emissionColor;
}*/


        // Next ray segment
        Vectorf reflectionColor = rayTrace(reflectionRay, depth + 1);

        // Next ray segment
        Ray refractionRay;
        refractionRay.direction = computeRefractionRay(
            shape->refraction,
            ray.direction,
            nHit,
            inside);
        refractionRay.origin = pHit - nHit * bias;
        // recurse
        Vectorf refractionColor = rayTrace(refractionRay, depth + 1);
        //float Kr, Kt;
        //fresnel(shape->refraction, nHit, ray.direction, &Kr, &Kt);

        float fresnelEffect = reflectAndRefract(pow(1 - facingratio, 3), 1, 0.1);
        surfaceColor = reflectionColor * fresnelEffect + refractionColor * (1-fresnelEffect) * shape->surfaceColor;








    else {
        // Case: diffuse/light/opaque
        for(int i = 0; i < allShapes.size(); i++) {
            if(allShapes[i].emissionColor.isNonZero()) {
                //found a light
                Vectorf transmission = 1;
                Vectorf lightDir = allShapes[i].center - pHit;
                lightDir.normalize();

                for(int j = 0; j < allShapes.size(); j++) {
                    if (i!=j) {
                        Vectorf pH, nH;
                        Ray lightRay;
                        lightRay.origin = pHit + nHit * bias;
                        lightRay.direction = lightDir;
                        if(allShapes[j].intersect(lightRay, &distance))
                            transmission = 0;
                            break;
                    }
                }

                surfaceColor += shape->surfaceColor * transmission * std::max(0.0f, nHit.dot(lightDir)) * allShapes[i].emissionColor;
            }
        }

    }

    return surfaceColor + shape->emissionColor;

    // CASE: Diffuse + Opaque
    // compute illumination, don't recurse any further
    /*Ray shadowRay;
    shadowRay.direction = lightPosition - pHit;
    bool isShadow = false;
    for (int k = 0; k < objects.size(); ++k) {
        if (Intersect(objects[k], shadowRay)) {
            // hit point is in shadow so just return
            return Vectorf(0);
        }
    }*/


    // point is illuminated
    //return object.color * light.brightness;



class Shape {
    public:
        enum {
            SPHERE =1,
            PLANE,
            ELLIPSE,
            RECTANGLE,
            LIGHT
        };
        int type;
        Vectorf center;
        Vectorf surfaceColor, emissionColor;
        float transparency, reflection, radius, radius2, refraction;
        Shape(const Vectorf &c,
            const float &r,
            const Vectorf &sc,
            const float &refl = 0,
            const float &refr = 1,
            const float &transp = 0,
            const Vectorf &ec = 0,
            int typeShape = SPHERE) :
            center(c), radius(r), radius2(r * r), surfaceColor(sc), emissionColor(ec),
            transparency(transp), reflection(refl), type(typeShape), refraction(refr) {}

        bool intersect(const Ray &ray, float *d) {
            switch(type) {
                case SPHERE:
                    // use geometric method
                    Vectorf oc = center - ray.origin;
                    // square distance to center of sphere
                    float oc2 = oc.dot(oc);
                    // distance to point on ray closest to sphere center
                    float tca = oc.dot(ray.direction);

                    bool outside = oc2 > radius2;
                    if (tca < 0 && outside) return 0;

                    // square distance from sphere center to closest point on ray
                    float d2 = oc2 - tca*tca;
                    // square distance from perpendicular bisector to center
                    float thc = radius2 - d2;

                    if (thc < 0)
                        return 0;
                    if (outside)
                        *d = tca - sqrtf(thc);
                    else
                        *d = tca + sqrtf(thc);

                    if (*d < 0) return 0;
                    return 1;
                    break;

            }
            return false;
        }

        Vectorf getNormal(Vectorf pHit) {
            switch(type) {
                case SPHERE:
                    Vectorf nHit;
                    nHit.x = pHit.x - center.x;
                    nHit.y = pHit.y - center.y;
                    nHit.z = pHit.z - center.z;
                    nHit.normalize();
                    return nHit;
                    break;

            }
        }

        bool isTransparent() {
            return (transparency>0);
        }




};
/*class Sphere : public Shape {
    public:
        Vectorf center;                           /// position of the sphere
        float radius, radius2;                  /// sphere radius and radius^2
        Vectorf surfaceColor, emissionColor;      /// surface color and emission (light)
        float transparency, reflection;         /// surface transparency and reflectivity

        //Constructor
        Sphere(
            const Vectorf &c,
            const float &r,
            const Vectorf &sc,
            const float &refl = 0,
            const float &transp = 0,
            const Vectorf &ec = 0) :
            center(c), radius(r), radius2(r * r), surfaceColor(sc), emissionColor(ec),
            transparency(transp), reflection(refl)
            { type = SPHERE;}

        /*bool intersect(const Vectorf &rayorig, const Vectorf &raydir, float &t0, float &t1) const {
            Vectorf l = center - rayorig;
            float tca = l.dot(raydir);
            if (tca < 0) return false;
            float d2 = l.dot(l) - tca * tca;
            if (d2 > radius2) return false;
            float thc = sqrt(radius2 - d2);
            t0 = tca - thc;
            t1 = tca + thc;

            return true;
        }*/
/*        bool intersect(Ray ray, Vectorf pHit, Vectorf nHit) {
            switch(type) {
                case SPHERE:
                    float t0 = INFINITY, t1 = INFINITY, tnear = INFINITY;
                    Vectorf l = center - ray.origin;
                    float tca = l.dot(ray.direction);
                    if (tca < 0) return false;
                    float d2 = l.dot(l) - tca * tca;
                    if (d2 > radius2) return false;
                    float thc = sqrt(radius2 - d2);
                    t0 = tca - thc;
                    t1 = tca + thc;

                    if(t0< 0)
                        t0 = t1;
                    if(t0 < tnear) {
                        tnear = t0;
                    }

                    pHit = ray.origin + ray.direction * tnear;
                    nHit = pHit - center;


                    return true;
            }
            return false;
        }

                            float t0 = INFINITY, t1 = INFINITY, minDist = INFINITY;
                    Vectorf l = center - ray.origin;
                    float tca = l.dot(ray.direction);
                    if (tca < 0) return false;
                    float d2 = l.dot(l) - tca * tca;
                    if (d2 > radius2) return false;
                    float thc = sqrt(radius2 - d2);
                    t0 = tca - thc;
                    t1 = tca + thc;

                    if(t0< 0)
                        t0 = t1;
                    if(t0 < minDist) {
                        minDist = t0;
                    }

                    pHit = ray.origin + ray.direction * minDist;
                    nHit = pHit - center;


                    return true;
};*/

Vectorf rayTrace(const Ray &ray, int depth)
{
	Object* shape = NULL;
	float minDist = INFINITY;
	Vectorf pHit; //point of intersection
	Vectorf nHit; //normal to the intersection
	bool inside = false;
	float bias = 1e-4;
	float distance;
	float t0 = INFINITY;
	float t1 = INFINITY;

	//hit the max, so we're at background at this point
	if (depth > max_recur) {
		return backgroundColor;
	}

	//calculate the intersections between the ray and the next object, we want the closest
	for (int i = 0; i < allShapes.size(); i++) {
		//if (Intersect(allShapes[i], ray, &pHit, &nHit)) {
		if (allShapes[i]->intersect(ray, &t0, &t1)) {
			if (distance < minDist) {
				if (t0 < 0)
					t0 = t1;
				if (t0 < minDist) {
					minDist = t0;
					shape = allShapes[i];
				}
			}
		}
	}

	if (shape == NULL)
		return backgroundColor;

	pHit = ray.origin + ray.direction * minDist;
	nHit = shape->getNormal(pHit);

	//check if inside (refracting)
	if (ray.direction.dot(nHit)>0) {
		nHit = -nHit;
		inside = true;
	}


	// CASE: Reflect + Refract
	if (shape->transparency > 0 && depth < max_recur) {
		float facingratio = -ray.direction.dot(nHit);

		//Reflect ray
		Ray reflectionRay;
		reflectionRay.origin = pHit + nHit * bias;

		//Refract ray
		Ray refractionRay;
		refractionRay.origin = pHit - nHit * bias;

		//compute snell's for new directions, coming from air n1= 1
		computeSnell(ray.direction, -nHit, 1.0, shape->indexOfRefraction, &reflectionRay.direction, &refractionRay.direction);

		//normalize rays
		refractionRay.direction.normalize();
		reflectionRay.direction.normalize();

		//compute for the fresnel effect
		float cosi = ray.direction.dot(-nHit);
		float cost = refractionRay.direction.dot(-nHit);
		float fresnelEffect = computeFresnelEffect(1.0, shape->indexOfRefraction, cosi, cost);

		//normalize the effect
		if (fresnelEffect < 0)
			fresnelEffect = 0;
		if (fresnelEffect > 1)
			fresnelEffect = 1;

		Vectorf reflColor = rayTrace(reflectionRay, depth + 1);
		Vectorf refrColor = rayTrace(reflectionRay, depth + 1);
		return shape->color * refrColor * (1 - fresnelEffect) + reflColor * fresnelEffect;
	}

	if (shape->reflectivity == 0) //matte
		return shape->color;

	// Case: shadow or light?
	Ray shadowRay;
	bool inShadow = false;

	//compute the shadow ray
	shadowRay.origin = Vectorf(pHit.x + nHit.x * bias, pHit.y + nHit.y * bias, pHit.z + nHit.z * bias);
	shadowRay.direction = shape->center - pHit;

	float length = shadowRay.direction.length();
	shadowRay.direction.normalize();

	float lengthDotN = shadowRay.direction.dot(nHit);
	if (lengthDotN < 0)
		return Vectorf(0);
	//compute light color
	Vectorf lightColor = allLights[0]->color;
	for (int i = 0; i < allShapes.size(); i++) {
		if (allShapes[i]->intersect(shadowRay, &t0, &t1) && !(allShapes[i]->isLight)) {
			if (allShapes[i]->transparency > 0) //transparent object attenuates the light color
				lightColor *= allShapes[i]->color;
			else
				inShadow = 1;
			break;
		}
	}

	lightColor *= 1.f / (length*length);
	return (inShadow) ? Vectorf(0) : shape->color * lightColor * lengthDotN;
}
