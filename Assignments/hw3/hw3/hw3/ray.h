#pragma once
#include "vector.h"

/*
	Class: Ray
	A ray that will we be used to trace paths for lighting, color, etc.
*/
namespace RayTracer {
	class Ray {
		public:
			//Constructor
			Ray() : m_vOrigin(Vector()), m_vDir(Vector()) {}; 
			Ray(Vector& vOrigin, Vector& vDir) : m_vOrigin(vOrigin), m_vDir(vDir) {};
			void SetOrigin(Vector& a_Origin) { m_vOrigin = a_Origin; }
			void SetDirection(Vector& a_Direction) { m_vDir = a_Direction; }
			Vector& GetOrigin() { return m_vOrigin; }
			Vector& GetDirection() { return m_vDir; }
		private:
			Vector m_vOrigin;
			Vector m_vDir;
	};
}