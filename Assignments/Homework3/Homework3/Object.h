#include "Vector.h"
#include <iostream>
#include <stdio.h>

typedef enum SHAPE_TYPE {SPHERE, PLANE, ELLIPSE, RECTANGLE};
class Object {
    public:
        Vectorf center;
        Vectorf color;
        bool isLight;
        float indexOfRefraction;
		float transparency;
		float reflectivity;
		Object(Vectorf c = Vectorf(0), Vectorf col = Vectorf(1), float n = 1.3,
				float refl = 0, float tran = 0, bool l = false) :
                color(col), indexOfRefraction(n), isLight(l), center(c),
				transparency(tran), reflectivity(refl) {}

        virtual bool intersect(const Ray &ray, float *t0, float *t1) {return false;}

		virtual Vectorf getNormal(Vectorf pHit) { return Vectorf(); }

        void setColor(Vectorf col) {color = col;}
        Vectorf getColor() {return color;}

        void setIndexOfRefraction(float index) {indexOfRefraction = index;}
        float getIndexOfRefraction() {return indexOfRefraction;}

        void setLight(bool is) {isLight = is;}
        bool getIsLight() {return isLight;}

		void setTransparency(float t) { transparency = t; }
		float getTransparency() { return transparency; }

		void setReflectivity(float t) { reflectivity = t; }
		float getReflectivity() { return reflectivity; }
};

class Sphere : public Object {
    public:
        Vectorf center;
        float radius, radius2;

        Sphere (Vectorf c = Vectorf(0), float r = 1, Vectorf col = Vectorf(1), float n = 1,
			float refl = 0, float tran = 0, bool l = false) : center(c), radius(r), radius2(r*r) {
                    setColor(col);
                    setIndexOfRefraction(n);
                    setLight(l);
					setTransparency(tran);
					setReflectivity(refl);
                }

        bool intersect(const Ray &ray, float *t0, float *t1) {
            Vectorf l = center - ray.origin;
            float tca = l.dot(ray.direction);
            if (tca < 0)
                return false;
            float d2 = l.dot(l) - (tca * tca);
            if (d2 > radius2)
                return false;
            float thc = sqrt(radius2 - d2);
            *t0 = tca - thc;
            *t1 = tca + thc;

            return true;
        }

        Vectorf getNormal(Vectorf pHit) {
            Vectorf nHit;
            nHit.x = pHit.x - center.x;
            nHit.y = pHit.y - center.y;
            nHit.z = pHit.z - center.z;
            nHit.normalize();
            return nHit;
        }



};

//pthurston@scu.edu
