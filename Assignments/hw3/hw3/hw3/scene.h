#pragma once
#include "Object.h"
namespace RayTracer {
	class Scene
	{
		public:
			//Constructor
			Scene() : m_oObject(0), m_iObjects(0) {};
			//Destructor
			~Scene();

			//Methods
			void initScene();
			int getNumObjects() { return m_iObjects; }
			Object* getObject(int key) { return m_oObject[key]; }

		private:
			int m_iObjects;
			Object** m_oObject;
	};
};