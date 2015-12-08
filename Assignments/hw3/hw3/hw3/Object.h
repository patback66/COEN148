#pragma once
#include "vector.h"
#include "color.h"
#include "ray.h"
#include "material.h"
#include "plane.h"

namespace RayTracer {
	class Object
	{
		public:
			enum
			{
				SPHERE = 1,
				PLANE,
				ELLIPSE
			};
			Object() : m_Name(0), m_Light(false) {};
			Material* GetMaterial() { return &m_Mat; }
			void SetMaterial(Material& mat) { m_Mat = mat; }
			virtual int GetType() = 0;
			virtual int Intersect(Ray& ray, float& distance) = 0;
			virtual Vector GetNormal(Vector& a_Pos) = 0;
			virtual Color GetColor(Vector&) { return m_Mat.GetColor(); }
			virtual void Light(bool a_Light) { m_Light = a_Light; }
			bool IsLight() { return m_Light; }
			void SetName(char* a_Name);
			char* GetName() { return m_Name; }
		protected:
			Material m_Mat;
			char* m_Name;
			bool m_Light;
	};


	class Sphere : public Object
	{
		public:
			int GetType() { return SPHERE; }
			Sphere(Vector& a_Centre, float a_Radius) :
				m_Centre(a_Centre), m_SqRadius(a_Radius * a_Radius),
				m_Radius(a_Radius), m_RRadius(1.0f / a_Radius) {};
			Vector& GetCentre() { return m_Centre; }
			float GetSqRadius() { return m_SqRadius; }
			int Intersect(Ray& a_Ray, float& a_Dist);
			Vector GetNormal(Vector& a_Pos) { return (a_Pos - m_Centre) * m_RRadius; }
		private:
			Vector m_Centre;
			float m_SqRadius, m_Radius, m_RRadius;
	};

	//class ellipse

	class Plane : public Object
	{
		public:
			int GetType() { return PLANE; }
			Plane(Vector& a_Normal, float a_D) : m_Plane(plane(a_Normal, a_D)) {};
			Vector& GetNormal() { return m_Plane.N; }
			float GetD() { return m_Plane.D; }
			int Intersect(Ray& a_Ray, float& a_Dist);
			Vector GetNormal(Vector& a_Pos);
		private:
			plane m_Plane;
	};

};