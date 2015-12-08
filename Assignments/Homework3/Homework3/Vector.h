#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#define INFINITY 1e8

class Point {
    public:
        float x, y, z;
        Point() : x(0), y(0), z(0) {}
        Point(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
};

template<typename T>
class Vector {
    public:
        T x, y, z;
        Vector() : x(T(0)), y(T(0)), z(T(0)) {}
        Vector(T xx) : x(xx), y(xx), z(xx) {}
        Vector(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
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
        T dot(const Vector<T> &v) const { return x * v.x + y * v.y + z * v.z; }
        Vector<T> operator - (const Vector<T> &v) const { return Vector<T>(x - v.x, y - v.y, z - v.z); }
        Vector<T> operator + (const Vector<T> &v) const { return Vector<T>(x + v.x, y + v.y, z + v.z); }
        Vector<T>& operator += (const Vector<T> &v) { x += v.x, y += v.y, z += v.z; return *this; }
        Vector<T>& operator *= (const Vector<T> &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
        Vector<T> operator - () const { return Vector<T>(-x, -y, -z); }
        T length2() const { return x * x + y * y + z * z; }
        T length() const { return sqrt(length2()); }
        friend std::ostream & operator << (std::ostream &os, const Vector<T> &v)
        {
            os << "[" << v.x << " " << v.y << " " << v.z << "]";
            return os;
        }
        bool isNonZero() { return ((x>0)||(y>0)||(z>0)); }
};
typedef Vector<float> Vectorf;

class Ray {
    public:
        Vectorf origin;
        Vectorf direction;
        Ray() : origin(Vectorf()), direction(Vectorf()) {}
        Ray(Vectorf orig, Vectorf dir) : origin(orig), direction(dir) {}
};

#endif // VECTOR_H_INCLUDED
