//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#include "Sphere.h"
#include "FloatsComparision.h"
#include "RootFinding.h"

bool Luxko::Sphere::FrontIntersect(const Line3DH& line, Point3DH& at) const
{
	auto ObjectSpaceTransform = GetObjectSpaceTransform();
	auto lineOS = ObjectSpaceTransform.ApplyOnLine(line);

	Vector3f S = lineOS._S.AsVector4f().xyz();
	Vector3f V = lineOS.Orientation().AsVector4f().xyz();

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
