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

bool Luxko::Ellipsoid::GetFirstIfIntersect(const Line3DH& line, Point3DH& at) const
{
	auto ObjectSpaceTransform = TransformWtoO();
	auto lineOS = ObjectSpaceTransform.ApplyOnLine(line);

	auto by = _mm_set_ps(1.f, _m, _n, 0.f);
	Vector4f S, V;
	S._m128 = _mm_mul_ps(lineOS._S.AsVector4f()._m128, by);
	V._m128 = _mm_mul_ps(lineOS.Orientation().AsVector4f()._m128, by);

	float a = V.MagSquare();
	float b = S.Dot(V)*2.f;
	float c = S.MagSquare() - _r*_r;
	auto roots = SolveQuadraticPolynomials(b / a, c / a);
	
	if (roots.count == 1U) {
		roots[1] = -1.f;
	}
	if (roots.count == 2U) {
		auto r1 = roots[0];
		auto r2 = roots[1];
		auto mini = min(r1, r2);
		auto maxi = max(r1, r2);
		float t;
		if (maxi < 0.f)return false;
		if (mini < 0.f) t = maxi;
		else {
			t = mini;
		}
		at = (ObjectSpaceTransform.Inverse())*(lineOS(t));
		return true;
	}
	return false;
}

bool Luxko::Ellipsoid::Contain(const Point3DH& p) const
{
	auto pOS = ToObjectSpace(p);

	auto x = pOS.x();
	auto y = pOS.y();
	auto z = pOS.z();

	auto evaluate = x*x + _m*_m*y*y + _n*_n*z*z;
	if (evaluate <= _r*_r) {
		return true;
	}

	return false;
}

