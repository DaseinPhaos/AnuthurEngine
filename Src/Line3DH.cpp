//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Line3DH.h"
#include "Plane3DH.h"
#include "FloatsComparision.h"

Luxko::Line3DH::Line3DH(const Point3DH& p, const Vector3DH& v)
{
	_S = p;
	_V = v.Normalize();
}

bool Luxko::Line3DH::operator==(const Line3DH& l) const
{
	if (l._V != _V) {
		return false;
	}

	if ((_S - l._S).Normalize() == _V) {
		return true;
	}
	return false;
}

Luxko::Point3DH Luxko::Line3DH::Intersect(const Plane3DH& p) const
{
	return p.Intersect(*this);
}

void Luxko::Line3DH::InitializeN(const Point3DH& p, const Vector3DH& v)
{
	_S = p;
	_V = v;
}

float Luxko::Line3DH::Distance(const Point3DH& p) const noexcept
{
	auto sp = p - _S;
	return (sp - (sp*_V)*_V).Length();
}

float Luxko::Line3DH::Distance(const Plane3DH& p) const noexcept
{
	return p.Distance(*this);
}

float Luxko::Line3DH::Distance(const Line3DH& l) const noexcept
{
	if (Parallel(l)) {
		return Distance(l._S);
	}

	auto M00 = -(l._V*l._V);
	auto M01 = _V*l._V;
	auto M10 = -M01;
	auto M11 = _V*_V;
	auto v0 = (l._S - _S)*_V;
	auto v1 = (l._S - _S)*l._V;
	auto det = M01*M01 + M11*M00;
	auto t0 = (M00*v0 + M01*v1) / det;
	auto P0 = _S + t0*_V;
	auto t1 = (M10*v0 + M11*v1) / det;
	auto P1 = l._S + t1 * l._V;
	return (P1 - P0).Length();
}

bool Luxko::Line3DH::Contain(const Point3DH& p) const noexcept
{
	auto v = (p - _S).Normalize();
	return v == _V || -v == _V;
}

bool Luxko::Line3DH::Parallel(const Plane3DH& p) const noexcept
{
	auto dot = _V*p.GetNormal();
	if (AlmostEqualRelativeAndAbs(dot, 0.f)) {
		return true;
	}
	return false;
}

bool Luxko::Line3DH::Parallel(const Vector3DH& v) const noexcept
{
	return _V.Parallel(v);
}

bool Luxko::Line3DH::Parallel(const Line3DH& l) const noexcept
{
	if (_V == l._V) {
		return true;
	}
	if (-_V == l._V) {
		return true;
	}
	return false;
}

bool Luxko::Line3DH::skew(const Line3DH& l) const noexcept
{
	if (Parallel(l)) {
		return false;
	}

	auto dis = Distance(l);
	if (AlmostEqualRelativeAndAbs(dis, 0.f)) {
		return false;
	}

	return true;
}

bool Luxko::Line3DH::Perpendicular(const Line3DH& l) const noexcept
{
	auto dot = _V*l._V;
	if (AlmostEqualRelativeAndAbs(dot, 0.f)) {
		return true;
	}
	return false;
}

Luxko::Point3DH Luxko::Line3DH::operator()(float t) const
{
	return _S + t*_V;
}

bool Luxko::Line3DH::Perpendicular(const Vector3DH& v) const noexcept
{
	return AlmostEqualRelativeAndAbs(_V*v, 0.f);
}

bool Luxko::Line3DH::Perpendicular(const Plane3DH& p) const noexcept
{
	if (_V == p.GetNormal()) {
		return true;
	}
	if (-_V == p.GetNormal()) {
		return true;
	}
	return false;
}

void Luxko::Line3DH::Orientation(const Vector3DH& v)
{
	_V = v.Normalize();
}

void Luxko::Line3DH::OrientationN(const Vector3DH& vN)
{
	_V = vN;
}
