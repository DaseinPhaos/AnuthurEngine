//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Cone.h"
#include "Vector3f.h"
#include "RootFinding.h"

bool Luxko::Cone::GetFirstIfIntersect(const Line3DH& line, Point3DH& at) const
{
	auto OST = TransformWtoO();
	auto lineOS = OST.ApplyOnLine(line);

	auto intersectionXY_t = -lineOS._S.z() / lineOS.Orientation().z();
	if (intersectionXY_t >= 0.f) {
		auto intersectionOS = lineOS(intersectionXY_t);
		at = (OST.Inverse())*intersectionOS;
		return true;
	}

	
	// Now intersection can only occurs on lateral surface
	auto s = Vector3f((lineOS._S.x()) / _a, (lineOS._S.y()) / _b, (lineOS._S.z()) / _h);
	auto v = Vector3f((lineOS.Orientation().x()) / _a, (lineOS.Orientation().y()) / _b, (lineOS.Orientation().z()) / _h);

	auto a = s.MagSquare();
	auto b = 2.f*(s.Dot(v) - v._z);
	auto c = v.MagSquare() + 1.f - 2.f*s._z;
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
		at = (OST.Inverse())*(lineOS(t));
		return true;
	}
	return false;
	
}

bool Luxko::Cone::Contain(const Point3DH& p) const
{
	auto pOS = ToObjectSpace(p);
	auto z = pOS.z();
	if (z<0.f || z>_h) {
		return false;
	}

	auto x = pOS.x();
	auto y = pOS.y();
	
	x = x / _a;
	y = y / _b;
	z = 1.f - z / _h;

	if (x*x + y*y <= z*z) {
		return true;
	}

	return false;
}

Luxko::Vector3DH Luxko::Cone::GetNormalAt_ObjectSpace(float x, float y, float z) const
{
	return Vector3DH(2.f*x / (_a*_a), 2.f*y / (_b*_b), (2.f*z / (_h*_h)) - 2.f / _h);
}
