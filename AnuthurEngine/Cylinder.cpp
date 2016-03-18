//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Cylinder.h"
#include "FloatsComparision.h"
#include "RootFinding.h"

bool Luxko::Cylinder::FrontIntersect(const Line3DH& line, Point3DH& at) const
{
	auto ObjectSpaceTransform = GetObjectSpaceTransform();
	auto lineOS = ObjectSpaceTransform.ApplyOnLine(line);

	auto m = _r / _s;
	auto S = line._S.AsVector4f();
	auto V = line.Orientation().AsVector4f();

	auto a = V._x*V._x + m*m*V._y*V._y;
	auto b = 2.f*(S._x*V._x + m*m*S._y*V._y);
	auto c = S._x*S._x + m*m*S._y*S._y - _r*_r;

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
