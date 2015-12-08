#pragma once
#include <math.h>
/*
	Class: Vector
	Holds a Vector. Allows Vector math - normalize, length, dotproduct, crossproduct
*/
class Vector {
	public:
		//Constructor
		Vector() : x(0.0f), y(0.0f), z(0.0f) {}; //default constructor
		Vector(float a_X, float a_Y, float a_Z) : x(a_X), y(a_Y), z(a_Z) {}; //arbitrary constructor
		void Set(float a_X, float a_Y, float a_Z) { x = a_X; y = a_Y; z = a_Z; } //modify Vector components

		//
		void Normalize() { float l = 1.0f / getLength(); x *= l; y *= l; z *= l; }
		float getLength() { return (float)sqrt(x * x + y * y + z * z); }
		float SqrLength() { return x * x + y * y + z * z; }
		float DotProduct(Vector v) { return x * v.x + y * v.y + z * v.z; }
		Vector CrossProduct(Vector v) { return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
		
		//Vector math operators
		void operator += (Vector& a_V) { x += a_V.x; y += a_V.y; z += a_V.z; }
		void operator += (Vector* a_V) { x += a_V->x; y += a_V->y; z += a_V->z; }
		void operator -= (Vector& a_V) { x -= a_V.x; y -= a_V.y; z -= a_V.z; }
		void operator -= (Vector* a_V) { x -= a_V->x; y -= a_V->y; z -= a_V->z; }
		void operator *= (float f) { x *= f; y *= f; z *= f; }
		void operator *= (Vector& a_V) { x *= a_V.x; y *= a_V.y; z *= a_V.z; }
		void operator *= (Vector* a_V) { x *= a_V->x; y *= a_V->y; z *= a_V->z; }
		Vector operator- () const { return Vector(-x, -y, -z); }
		
		friend Vector operator + (const Vector& vA, const Vector& vB) { return Vector(vA.x + vB.x, vA.y + vB.y, vA.z + vB.z); }
		friend Vector operator - (const Vector& vA, const Vector& vB) { return Vector(vA.x - vB.x, vA.y - vB.y, vA.z - vB.z); }
		friend Vector operator + (const Vector& vA, Vector* vB) { return Vector(vA.x + vB->x, vA.y + vB->y, vA.z + vB->z); }
		friend Vector operator - (const Vector& vA, Vector* vB) { return Vector(vA.x - vB->x, vA.y - vB->y, vA.z - vB->z); }
		friend Vector operator * (const Vector& v, float f) { return Vector(v.x * f, v.y * f, v.z * f); }
		friend Vector operator * (const Vector& vA, Vector& vB) { return Vector(vA.x * vB.x, vA.y * vB.y, vA.z * vB.z); }
		friend Vector operator * (float f, const Vector& v) { return Vector(v.x * f, v.y * f, v.z * f); }
		
		union {
			struct { float x, y, z; };
			struct { float r, g, b; };
			struct { float cell[3]; };
		};
};