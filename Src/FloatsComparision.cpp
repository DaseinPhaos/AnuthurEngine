//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "FloatsComparision.h"



bool Luxko::IsValidFloat(float f)
{
	return !std::isnan(f);
}

bool Luxko::IsInfinityFloat(float f)
{
	return std::isinf(f);
}

bool Luxko::AlmostEqualRelativeAndAbs(float x, float y, 
	float maxDiff /*= 2 * FLT_EPSILON*/, float maxRelDiff /*= 2 * FLT_EPSILON*/)
{
	float diff = std::fabsf(x - y);
	if (diff < maxDiff) {
		return true;
	}
	x = std::fabsf(x);
	y = std::fabsf(y);
	auto maxF = max(x, y);
	if (diff < maxF*maxRelDiff) {
		return true;
	}
	return false;
}
