#pragma once
#include "CommonHeader.h"
#include <cfloat>
//#include <numeric>

namespace Luxko {
	constexpr float _LUX_ABS_EPSILON = 1.f / 0x40000;
	constexpr float _LUX_REL_EPSILON = 1.f / 0x4000;
	bool IsValidFloat(float f);
	bool IsInfinityFloat(float f);
	bool AlmostEqualRelativeAndAbs(float x, float y,
		float maxDiff = _LUX_ABS_EPSILON, float maxRelDiff = _LUX_REL_EPSILON);

}