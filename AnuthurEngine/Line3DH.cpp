#include "Line3DH.h"
#include "Plane3DH.h"
#include "FloatsComparision.h"

Luxko::Line3DH::Line3DH(const Point3DH& p, const Vector3DH& v)
{
	S = p;
	V = v.Normalize();
}

bool Luxko::Line3DH::operator==(const Line3DH& l) const
{
	if (l.V != V) {
		return false;
	}

	if ((S - l.S).Normalize() == V) {
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
	S = p;
	V = v;
}

float Luxko::Line3DH::Distance(const Point3DH& p) const noexcept
{
	auto sp = p - S;
	return (sp - (sp*V)*V).Length();
}

float Luxko::Line3DH::Distance(const Plane3DH& p) const noexcept
{
	return p.Distance(*this);
}

float Luxko::Line3DH::Distance(const Line3DH& l) const noexcept
{
	if (Parallel(l)) {
		return Distance(l.S);
	}

	auto M00 = -(l.V*l.V);
	auto M01 = V*l.V;
	auto M10 = -M01;
	auto M11 = V*V;
	auto v0 = (l.S - S)*V;
	auto v1 = (l.S - S)*l.V;
	auto det = M01*M01 + M11*M00;
	auto t0 = (M00*v0 + M01*v1) / det;
	auto P0 = S + t0*V;
	auto t1 = (M10*v0 + M11*v1) / det;
	auto P1 = l.S + t1 * l.V;
	return (P1 - P0).Length();
}

bool Luxko::Line3DH::Contain(const Point3DH& p) const noexcept
{
	auto v = (p - S).Normalize();
	return v == V || -v == V;
}

bool Luxko::Line3DH::Parallel(const Plane3DH& p) const noexcept
{
	auto dot = V*p.GetNormal();
	if (AlmostEqualRelativeAndAbs(dot, 0.f)) {
		return true;
	}
	return false;
}

bool Luxko::Line3DH::Parallel(const Vector3DH& v) const noexcept
{
	return V.Parallel(v);
}

bool Luxko::Line3DH::Parallel(const Line3DH& l) const noexcept
{
	if (V == l.V) {
		return true;
	}
	if (-V == l.V) {
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
	auto dot = V*l.V;
	if (AlmostEqualRelativeAndAbs(dot, 0.f)) {
		return true;
	}
	return false;
}

Luxko::Point3DH Luxko::Line3DH::operator()(float t) const
{
	return S + t*V;
}

bool Luxko::Line3DH::Perpendicular(const Vector3DH& v) const noexcept
{
	return AlmostEqualRelativeAndAbs(V*v, 0.f);
}

bool Luxko::Line3DH::Perpendicular(const Plane3DH& p) const noexcept
{
	if (V == p.GetNormal()) {
		return true;
	}
	if (-V == p.GetNormal()) {
		return true;
	}
	return false;
}

void Luxko::Line3DH::Orientation(const Vector3DH& v)
{
	V = v.Normalize();
}

void Luxko::Line3DH::OrientationN(const Vector3DH& vN)
{
	V = vN;
}
