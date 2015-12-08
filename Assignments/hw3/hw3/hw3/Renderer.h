#pragma once
#include "ray.h"
#include "color.h"
#include "pixel.h"

namespace RayTracer {
	class Scene;
	class Primitive;
	class Renderer
	{
		public:
			//Constructor
			Renderer();
			//Destructor
			~Renderer();

			void SetTarget(Pixel* a_Dest, int a_Width, int a_Height);
			Scene* GetScene() { return m_Scene; }
			Primitive* Raytrace(Ray& ray, Color& color, int Depth, float a_RIndex, float& a_Dist);
			void InitRender();
			bool Render();
		protected:
			// renderer data
			float m_WX1, m_WY1, m_WX2, m_WY2, m_DX, m_DY, m_SX, m_SY;
			Scene* m_Scene;
			Pixel* m_Dest;
			int m_Width, m_Height, m_CurrLine, m_PPos;
			Primitive** m_LastRow;
	};
};