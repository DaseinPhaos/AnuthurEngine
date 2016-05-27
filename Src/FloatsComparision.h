//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
#ifdef ANUTHURMATH_EXPORTS
#define ANUTHURMATH_API _declspec(dllexport)
#else
#define ANUTHURMATH_API _declspec(dllimport)
#endif
#include "CommonHeader.h"
#include <cfloat>
//#include <numeric>

namespace Luxko {
	constexpr float _LUX_ABS_EPSILON = 1.f / 0x40000;
	constexpr float _LUX_REL_EPSILON = 1.f / 0x10000;
	ANUTHURMATH_API bool IsValidFloat(float f);
	ANUTHURMATH_API bool IsInfinityFloat(float f);
	ANUTHURMATH_API bool AlmostEqualRelativeAndAbs(float x, float y,
		float maxDiff = _LUX_ABS_EPSILON, float maxRelDiff = _LUX_REL_EPSILON);

}