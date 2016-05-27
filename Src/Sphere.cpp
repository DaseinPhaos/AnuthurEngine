//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#include "Sphere.h"
#include "FloatsComparision.h"
#include "RootFinding.h"

bool Luxko::Sphere::GetFirstIfIntersect(const Line3DH& line, Point3DH& at) const
{
	auto ObjectSpaceTransform = TransformWtoO();
	auto lineOS = ObjectSpaceTransform.ApplyOnLine(line);

	Vector3f S = lineOS._S.AsVector4f().xyz();
	Vector3f V = lineOS.Orientation().AsVector4f().xyz();

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

bool Luxko::Sphere::Contain(const Point3DH& p) const
{
	auto evaluate = p.AsVector4f().MagSquare();
	return (evaluate < _r*_r);
}


