//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Triangle3DH.h"
#include "FloatsComparision.h"

Luxko::Triangle3DH::Triangle3DH(const Point3DH& p1, const Point3DH& p2, const Point3DH& p3)
{
	_P0 = p1;
	_P1 = p2;
	_P2 = p3;
}

Luxko::Triangle3DH::Triangle3DH(const Point3DH* points)
{
	_P0 = points[0];
	_P1 = points[1];
	_P2 = points[2];
}

Luxko::Vector3DH Luxko::Triangle3DH::GetNormal() const
{
	return ((_P1 - _P0).Cross(_P2 - _P0)).Normalize();
}

Luxko::Plane3DH Luxko::Triangle3DH::GetPlane() const
{
	auto normal = GetNormal();
	Plane3DH result;
	result.InitializeN(_P0, normal);
	return result;
}

bool Luxko::Triangle3DH::Intersect(const Line3DH& line, Point3DH& at) const
{
	auto normal = GetNormal();
	Plane3DH plane;
	plane.InitializeN(_P0, normal);

	if (line.Perpendicular(normal)) {
		// CAUTION: containing is out.
		return false;
	}
	else {
		auto point = line.Intersect(plane);
		auto R = point - _P0;
		auto Q1 = _P1 - _P0;
		auto Q2 = _P2 - _P0;

		auto Q1Sq = Q1*Q1;
		auto Q2Sq = Q2*Q2;
		auto Q1dotQ2 = Q1*Q2;
		auto Det = Q1Sq*Q2Sq - Q1dotQ2*Q1dotQ2;

		auto RdotQ1 = R*Q1;
		auto RdotQ2 = R*Q2;

		auto w1 = (Q2Sq*RdotQ1 - Q1dotQ2*RdotQ2) / Det;

		auto w2 = (Q1Sq*RdotQ2 - Q1dotQ2*RdotQ1) / Det;
		if (w1 < 0.f || w2<0.f || w1 + w2>1.f) {
			return false;
		}
		else {
			at = point;
			return true;
		}
	}

}

bool Luxko::Triangle3DH::IntersectBarycentric(const Line3DH& line, Vector3f& barycentricCoordinates) const
{
	auto normal = GetNormal();
	Plane3DH plane;
	plane.InitializeN(_P0, normal);

	if (line.Perpendicular(normal)) {
		// CAUTION: containing is out.
		return false;
	}
	else {
		auto point = line.Intersect(plane);
		auto R = point - _P0;
		auto Q1 = _P1 - _P0;
		auto Q2 = _P2 - _P0;

		auto Q1Sq = Q1*Q1;
		auto Q2Sq = Q2*Q2;
		auto Q1dotQ2 = Q1*Q2;
		auto Det = Q1Sq*Q2Sq - Q1dotQ2*Q1dotQ2;

		auto RdotQ1 = R*Q1;
		auto RdotQ2 = R*Q2;

		auto w1 = (Q2Sq*RdotQ1 - Q1dotQ2*RdotQ2) / Det;

		auto w2 = (Q1Sq*RdotQ2 - Q1dotQ2*RdotQ1) / Det;
		auto w0 = 1.f - w1 - w2;
		if (w1 < 0.f || w2 < 0.f || w0 < 0.f) {
			return false;
		}
		else {
			barycentricCoordinates._x = w0;
			barycentricCoordinates._y = w1;
			barycentricCoordinates._z = w2;
			return true;
		}
	}

}

const Luxko::Point3DH& Luxko::Triangle3DH::operator[](size_t index) const
{
	if (index > 2U)
		throw;
	if (index == 0)return _P0;
	if (index == 1)return _P1;
	return _P2;
}

Luxko::Point3DH& Luxko::Triangle3DH::operator[](size_t index)
{
	if (index > 2U)
		throw;
	if (index == 0)return _P0;
	if (index == 1)return _P1;
	return _P2;
}

bool Luxko::Triangle3DH::operator==(const Triangle3DH& t)
{
	return _P0 == t._P0 && _P1 == t._P1 && _P2 == t._P2;
}
