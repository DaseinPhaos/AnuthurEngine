//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Ellipsoid.h"
#include "FloatsComparision.h"
#include "RootFinding.h"
#include <emmintrin.h>

bool Luxko::Ellipsoid::FrontIntersect(const Line3DH& line, Point3DH& at) const
{
	auto ObjectSpaceTransform = GetObjectSpaceTransform();
	auto lineOS = ObjectSpaceTransform.ApplyOnLine(line);

	auto by = _mm_set_ps(1.f, _m, _n, 0.f);
	Vector4f S, V;
	S._m128 = _mm_mul_ps(lineOS._S.AsVector4f()._m128, by);
	V._m128 = _mm_mul_ps(lineOS.Orientation().AsVector4f()._m128, by);

	float a = V.MagSquare();
	float b = S.Dot(V)*2.f;
	float c = S.MagSquare() - _r*_r;
	auto roots = SolveQuadraticPolynomials(b / a, c / a);
	
	if (roots.count >= 1U) {
		if (roots[0] > 0.f) {
			auto pointOS = lineOS(roots[0]);
			at = (ObjectSpaceTransform.Inverse())*pointOS;
			return true;
		}

		if (roots.count == 2U && roots[1] > 0.f) {
			auto pointOS = lineOS(roots[1]);
			at = (ObjectSpaceTransform.Inverse())*pointOS;
			return true;
		}
	}
	return false;
}
