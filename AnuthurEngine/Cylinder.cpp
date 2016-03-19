//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Cylinder.h"
#include "FloatsComparision.h"
#include "RootFinding.h"

bool Luxko::Cylinder::GetFirstIfIntersect(const Line3DH& line, Point3DH& at) const
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

bool Luxko::Cylinder::Contain(const Point3DH& p) const
{
	auto pOS = ToObjectSpace(p);
	auto z = p.z();
	if (z >= 0.f&&z <= _h) {
		auto x = p.x();
		auto y = p.y();
		auto rdivs = _r / _s;
		auto evaluate = x*x + rdivs*rdivs*y*y;
		if (evaluate <= _r*_r) {
			return true;
		}
		
	}
	return false;
}

Luxko::Vector3DH Luxko::Cylinder::GetNormalAt_ObjectSpace(float x, float y, float z) const
{
	auto rdivs = _r / _s;
	return Vector3DH(2.f*x, 2.f*y*rdivs*rdivs, 0.f).Normalize();
}

