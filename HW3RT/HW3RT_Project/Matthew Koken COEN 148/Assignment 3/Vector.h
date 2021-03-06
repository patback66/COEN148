#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#ifndef INFINITY
#define INFINITY 1e8
#endif
#include <stdlib.h>
class Point {
    public:
        double x, y, z;
        Point() : x(0), y(0), z(0) {}
        Point(double X, double Y, double Z) : x(X), y(Y), z(Z) {}
};

// Vector class, has arithmetic operations, dot product,
template<typename T>
class Vector {
    public:
        T x, y, z;
        Vector() : x(T(0)), y(T(0)), z(T(0)) {}
        Vector(T xx) : x(xx), y(xx), z(xx) {}
        Vector(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}

		//normalize a vector to length 1
        Vector& normalize()
        {
            T nor2 = length2();
            if (nor2 > 0) {
                T invNor = 1.f / sqrtf(nor2);
                x *= invNor, y *= invNor, z *= invNor;
            }
            return *this;
        }
        Vector<T> operator * (const T &f) const { return Vector<T>(x * f, y * f, z * f); }
        Vector<T> operator * (const Vector<T> &v) const { return Vector<T>(x * v.x, y * v.y, z * v.z); }
        T dot(const Vector<T> &v) const { return x * v.x + y * v.y + z * v.z; } //dot product for the vector
        Vector<T> operator - (const Vector<T> &v) const { return Vector<T>(x - v.x, y - v.y, z - v.z); }
        Vector<T> operator + (const Vector<T> &v) const { return Vector<T>(x + v.x, y + v.y, z + v.z); }
        Vector<T>& operator += (const Vector<T> &v) { x += v.x, y += v.y, z += v.z; return *this; }
        Vector<T>& operator *= (const Vector<T> &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
        Vector<T> operator - () const { return Vector<T>(-x, -y, -z); }
        T length2() const { return x * x + y * y + z * z; }
        T length() const { return sqrt(length2()); }

        bool isNonZero() { return ((x>0)||(y>0)||(z>0)); }
};
typedef Vector<double> Vectorf; //typedef for commonly used vector


// Class ray. Ray has an origin and direction vector
class Ray {
    public:
        Vectorf origin;
        Vectorf direction;
        Ray() : origin(Vectorf(0)), direction(Vectorf(0)) {}
        Ray(Vectorf orig, Vectorf dir) : origin(orig), direction(dir) {}
};

#endif // VECTOR_H_INCLUDED
