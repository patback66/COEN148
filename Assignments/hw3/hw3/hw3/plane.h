#pragma once
#include "vector.h"
class plane
{
	public:
		plane() : N(0, 0, 0), D(0) {};
		plane(Vector a_Normal, float a_D) : N(a_Normal), D(a_D) {};
		union
		{
			struct
			{
				Vector N;
				float D;
			};
			float cell[4];
		};
};