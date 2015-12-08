#pragma once
#include "color.h"

class Material
{
	public:
		//Constructor
		Material();

		//Methods
		void SetColor(Color& color) { m_oColor = color; }
		Color GetColor() { return m_oColor; }
		void SetDiffuse(float diff) { m_fDiff = diff; }
		void SetReflection(float a_Refl) { m_fRefl = a_Refl; }
		float GetSpecular() { return 1.0f - m_fDiff; }
		float GetDiffuse() { return m_fDiff; }
		float GetReflection() { return m_fRefl; }
	private:
		Color m_oColor;
		float m_fRefl;
		float m_fDiff;
};