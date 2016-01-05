#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include "Vector.h"
#include <math.h>

enum matType {kDiffuse, kPhong};

//class for an object
//object has a center, index of refraction, reflectivity, transparency, emission, etc.
class Object {
    public:
        Vectorf center;
        Vectorf color;
        double indexOfRefraction;
		Vectorf  emissionColor;    /// surface color and emission (light)
		double transparency, reflectivity;             /// surface transparency and reflectivity
		matType kMatType;
		Object(Vectorf c = Vectorf(0), Vectorf col = Vectorf(1), double n = 1.3,
				double refl = 0, double tran = 0, Vectorf ec = Vectorf(0)) :
                color(col), indexOfRefraction(n), center(c),
				transparency(tran), reflectivity(refl) {}

		virtual bool intersect(const Ray &ray, double *t0, double *t1) const = 0;
		virtual Vectorf getNormal(Vectorf pHit) const = 0;
		virtual Vectorf getLightDir(Vectorf pHit) const = 0;


        void setColor(Vectorf col) {color = col;}
        Vectorf getColor() {return color;}
		virtual Vectorf getColor(Vectorf point) const { return color;}

        void setIndexOfRefraction(double index) {indexOfRefraction = index;}
        double getIndexOfRefraction() {return indexOfRefraction;}

		void setTransparency(double t) { transparency = t; }
		double getTransparency() { return transparency; }

		void setReflectivity(double t) { reflectivity = t; }
		double getReflectivity() { return reflectivity; }

		void setEmissionColor(Vectorf ec) { emissionColor = ec; }
		Vectorf getEmissionColor() { return emissionColor; }

		void setCenter(Vectorf c) { center = c; }
		Vectorf getCenter() { return center; }

		void setMatType(matType k) { kMatType = k; }
		matType getMatType() { return kMatType; }
};

//class fora  sphere
class Sphere : public Object {
public:
	// Sphere variables.
	double radius, radius2;                      /// sphere radius and radius^2

	Sphere(const Vectorf &c, const double &r, const Vectorf &sc, const matType mt = kDiffuse,
		const double &refl = 0, const double &transp = 0, const double& n = 1,
		const Vectorf &ec = 0) : radius(r), radius2(r * r) {

		this->setTransparency(transp);
		this->setReflectivity(refl);
		this->setEmissionColor(ec);
		this->setCenter(c);
		this->setColor(sc);
		this->setIndexOfRefraction(n);
		this->setMatType(mt);

	}

	// compute a ray-sphere intersection using the geometric solution
	bool intersect(const Ray &ray, double *t0 = NULL, double *t1 = NULL) const
	{
        // x = o + d * l
		// we start with a vector (l) from the ray origin (rayorig) to the center of the curent sphere.
		Vectorf l = center - ray.origin;

		// tca -> vector of length given by normalized ray direction, perpendicular to l
		double tca = l.dot(ray.direction);

		//check if going the right direction, if not, no intersection
		if (tca < 0)
            return false;

		//So use pythagoras' theorem, should have an intersection

		// remove the length tca to the power of two (tca * tca) and we get a distance from the center of the sphere to the power of 2 (d2).
		double d2 = l.dot(l) - (tca * tca);
		// if the distance^2 is greater than the radius^2, there is no intersection
		if (d2 > radius2)
			return false;

		// yse Pythagoras' theorem again: radius2 is the hypotenuse and d2 is one of the side, get length thc
		double thc = sqrt(radius2 - d2);
		if (t0 != NULL && t1 != NULL) {
			// subtract thc from tca, get length from ray origin to surface intersection
			*t0 = tca - thc;
			// tca + thc gives the second intersection - the back side of the sphere
			*t1 = tca + thc;
		}
		return true;
	}

	//get the normal at point pHIt from the sphere
	Vectorf getNormal(Vectorf pHit) const {
		Vectorf nHit;
		nHit.x = pHit.x - this->center.x;
		nHit.y = pHit.y - this->center.y;
		nHit.z = pHit.z - this->center.z;
		nHit.normalize();
		return nHit;
	}

	//get the direction for the light at the point pHit
	Vectorf getLightDir(Vectorf pHit) const {
		return center - pHit;
	}
};

//class for an ellipsoid, incomplete
class Ellipsoid : public Object {
public:
	double a, a2, b, b2, c, c2;

	/*
	(x-Cx)^2/a + (y-Cy)^2/b + (z-Cz)^2/c = 1
	*/

	Ellipsoid(const Vectorf &c, const double &A, const double &B, const double &C, const Vectorf &sc,
		const matType &matType, const double &refl = 0, const double &transp = 0, const double& n = 1,
		const Vectorf &ec = Vectorf(0)) : a(A), a2(A*A), b(B), b2(B*B), c(C), c2(C*C) {

		this->setTransparency(transp);
		this->setReflectivity(refl);
		this->setEmissionColor(ec);
		this->setCenter(c);
		this->setColor(sc);
		this->setIndexOfRefraction(n);
		this->setMatType(matType);
	}

	// Ray - Ellipsoid intersection
	// PM = (X/a, y/b, z/c)
	// abs((P-C)M^2)=1
	// (PM - CM)dot(PM - CM) = 1
	// -> v1=vM, P1=P0M-CM
	// (P1 + tv1)dot(P1+tv1) = abs(P1)^2 + 2tP1dotv1 + t^2abs(v1)^2 = 1


};

#define OFFSET 25
//class for a plane
class Plane : public Object {
public:
	//Ax + By + Cz = D
	double a, b, c;
	Vectorf normal;

	Plane(const Vectorf &c, const double &A, const double &B, const double &C, const Vectorf &sc,
        const matType mt = kDiffuse, const double &refl = 0, const double &tran = 0,
        const double &n = 1, const Vectorf &ec = Vectorf(0)) : a(A), b(B), c(C), normal(Vectorf(A,B,C)){

		this->setTransparency(tran);
		this->setReflectivity(refl);
		this->setEmissionColor(ec);
		this->setCenter(c); //a point on the plane
		this->setColor(sc);
		this->setIndexOfRefraction(n);
		this->setMatType(mt);
		normal = Vectorf(A, B, C);
    }

    /*
        t = ( D - N dot P0)/(N dot v)
    */
    bool intersect(const Ray &ray, double *t0, double *t1) const {
        double denom = normal.dot(ray.direction);
        if (abs(denom) > double(0.0001))
        {
            double t = normal.dot(center - ray.origin) / denom;
			if (t >= 0) {
				*t0 = t;
				return true;
			}
        }
        return false;
    }

	//plane only has one normal
    Vectorf getNormal(Vectorf pHit) const { return normal; }

	//plane only has one normal
    Vectorf getLightDir(Vectorf pHit) const { return normal; }

	//give checkboard like texture
	/*Vectorf getColor(Vectorf point) const {
		bool x = (int)(point.x + OFFSET) % 5 == 0;
		bool y = (int)(point.y + OFFSET) % 5 == 0;
		bool z = (int)(point.z + OFFSET) % 5 == 0;

		if (x ^ y ^ z) {
			return Vectorf(0, 0, 0);
		}
		else {
			return Vectorf(1, 1, 1);
		}
	}*/

};

//get the sign of a value (+/-)
template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

//class for a box, incomplete
class Box : public Object {
public:
	Vectorf min, max; //gives two corners diagonally opposite, the other points can be extrapolated from there

	Box(const Vectorf &c, const Vectorf &mn, const Vectorf &mx, const Vectorf &sc, const matType mt = kDiffuse, const double &refl = 0,
		const double &tran = 0, const double &n = 1, const Vectorf &ec = Vectorf(0)) :
		min(mn), max(mx) {
		this->setCenter(c);
		this->setColor(sc);
		this->setMatType(mt);
		this->setReflectivity(refl);
		this->setTransparency(tran);
		this->setIndexOfRefraction(n);
		this->setEmissionColor(ec);
	}
	//largest ((a > b) ? a : b)
	//Using improved Ray-Box Intersection Algorithm by Amy Williams, Steve Barrus, R. Keith Morley, Peter Shirley at University of Utah
	//https://www.cs.utah.edu/~awilliam/box/box.pdf
	//This improved algorithm offered significantly faster runtimes than the previous, x y z plane case based approach
	virtual bool intersect(const Ray &ray, double *t0, double *t1) const {
		double tmin, tmax, tymin, tymax, tzmin, tzmax;
		Vectorf inv_dir = Vectorf(1 / ray.direction.x, 1 / ray.direction.y, 1 / ray.direction.z);
		int sign[] = { inv_dir.x < 0, inv_dir.y < 0, inv_dir.z < 0 };
		//bounds[0] = min, bounds[1] = max

		//x intersection boundaries
		tmin = ((sign[0]) ? max.x : min.x) - ray.origin.x * inv_dir.x;
		tmax = ((1 - sign[0]) ? max.x : min.x) - ray.origin.x * inv_dir.x;

		//y intersection boundaries
		tymin = ((sign[1]) ? max.y : min.y) - ray.origin.y * inv_dir.y;
		tymax = ((1 - sign[1]) ? max.y : min.y) - ray.origin.y * inv_dir.y;

		if ((tmin > tymax) || (tymin > tmax))
			return false;
		if (tymin > tmin)
			tmin = tymin;
		if (tymax < tmax)
			tmax = tymax;

		//z intersecion boundaries
		tzmin = ((sign[2]) ? max.z : min.z) - ray.origin.z * inv_dir.z;
		tzmax = ((1 - sign[2]) ? max.z : min.z) - ray.origin.z * inv_dir.z;

		if ((tmin > tzmax) || tzmin > tmax)
			return false;
		if (tzmin > tmin)
			tmin = tzmin;
		if (tzmax < tmax)
			tmax = tzmax;
		if ((tmin < *t1) && (tmax > *t0)) {
			*t0 = tmin;
			*t1 = tmax;
			return true;
		}
		return false;

		//old, slow version
		/*if (ray.direction.x >= 0) {
			tmin = (min.x - ray.origin.x) / ray.direction.x;
			tmax = (max.x - ray.origin.x) / ray.direction.x;
		}
		else {
			tmin = (max.x - ray.origin.x) / ray.direction.x;
			tmax = (min.x - ray.origin.x) / ray.direction.x;
		}
		if (ray.direction.y >= 0) {
			tymin = (min.y - ray.origin.y) / ray.direction.y;
			tymax = (max.y - ray.origin.y) / ray.direction.y;
		}
		else {
			tymin = (max.y - ray.origin.y) / ray.direction.y;
			tymax = (min.y - ray.origin.y) / ray.direction.y;
		}
		if ((tmin > tymax) || (tymin > tmax))
			return false;

		if (tymin > tmin)
				tmin = tymin;
		if (tymax < tmax)
			tmax = tymax;
		if (ray.direction.z >= 0) {
			tzmin = (min.z - ray.origin.z) / ray.direction.z;
			tzmax = (max.z - ray.origin.z) / ray.direction.z;
		}
		else {
			tzmin = (max.z - ray.origin.z) / ray.direction.z;
			tzmax = (min.z - ray.origin.z) / ray.direction.z;
		}
		if ((tmin > tzmax) || (tzmin > tmax))
			return false;
		if (tzmin > tmin)
			tmin = tzmin;
		if (tzmax < tmax)
			tmax = tzmax;
		if ((tmin < *t1) && (tmax > *t0)) {
			*t0 = tmin;
			*t1 = tmax;
			return true;
		}
		return false;*/
	}

	//get the normal for a box at a point, doesn't seem to work right
	Vectorf getNormal(Vectorf pHit) const {
		Vectorf normal;
		Vectorf localPoint = pHit - center;
		double min = std::numeric_limits<double>::max();
		double distance = std::abs(center.x - std::abs(localPoint.x));
		if (distance < min) {
			min = distance;
			normal = Vectorf(1, 0, 0);
			normal *= sgn(localPoint.x);
		}
		distance = std::abs(center.y - std::abs(localPoint.y));
		if (distance < min) {
			min = distance;
			normal = Vectorf(0, 1, 0);
			normal *= sgn(localPoint.y);
		}
		distance = std::abs(center.z - std::abs(localPoint.z));
		if (distance < min) {
			min = distance;
			normal = Vectorf(0, 0, 1);
			normal *= sgn(localPoint.z);
		}
		return normal;
	}

	Vectorf getLightDir(Vectorf pHit) const { return Vectorf(0, 0, 1); }
};


#endif // OBJECT_H_INCLUDED
