#include "Plane3DH.h"
#include "Matrix3x3f.h"
#include "FloatsComparision.h"

Luxko::Plane3DH::Plane3DH(const Point3DH* data)
{
	auto normal = (data[1] - data[0]).Cross(data[2] - data[1]).Normalize();
	InitializeN(data[0], normal);
}

Luxko::Plane3DH::Plane3DH(const Point3DH& p1, const Point3DH& p2, const Point3DH& p3)
{
	auto normal = (p2 - p1).Cross(p3 - p2).Normalize();
	InitializeN(p1, normal);
}

Luxko::Plane3DH::Plane3DH(const Point3DH& p, const Vector3DH& n)
{
	InitializeN(p, n.Normalize());
}

Luxko::Plane3DH::Plane3DH(const Vector4f& v)
{
	_L = v;
}

void Luxko::Plane3DH::InitializeN(const Point3DH& p, const Vector3DH& normal)
{
	_L = normal.AsVector4f();
	_L[3] = -p*normal;
}

Luxko::Plane3DH Luxko::Plane3DH::operator-() const
{
	Plane3DH result;
	result._L[0] = -_L[0];
	result._L[1] = -_L[1];
	result._L[2] = -_L[2];
	result._L[3] = _L[3];
	return result;
}

float Luxko::Plane3DH::Distance(const Point3DH& p) const noexcept
{
	return _L.Dot(p.AsVector4f());
}

float Luxko::Plane3DH::Distance(const Plane3DH& p) const noexcept
{
	return std::fabsf(_L[3] - p._L[3]);
}

float Luxko::Plane3DH::Distance(const Line3DH& l) const noexcept
{
	auto dot = GetNormal()*l.Orientation();
	if (AlmostEqualRelativeAndAbs(dot, 0.f)) {
		return Distance(l.S);
	}
	return 0.f;
}

bool Luxko::Plane3DH::Contain(const Point3DH& p) const noexcept
{
	auto dis = Distance(p);
	return AlmostEqualRelativeAndAbs(dis, 0.f);
}

bool Luxko::Plane3DH::Contain(const Line3DH& l) const noexcept
{
	if (!l.Perpendicular(GetNormal())) {
		return false;
	}
	if (!Contain(l.S)) {
		return false;
	}
	return true;
}

bool Luxko::Plane3DH::Parallel(const Vector3DH& v) const noexcept
{
	auto dot = v*GetNormal();
	return AlmostEqualRelativeAndAbs(dot, 0.f);
}

bool Luxko::Plane3DH::Parallel(const Plane3DH& p) const noexcept
{
	return GetNormal().Parallel(p.GetNormal());
}

bool Luxko::Plane3DH::Parallel(const Line3DH& l) const noexcept
{
	return Parallel(l.Orientation());
}

bool Luxko::Plane3DH::Perpendicular(const Line3DH& l) const noexcept
{
	return l.Orientation().Parallel(GetNormal());
}

bool Luxko::Plane3DH::Perpendicular(const Plane3DH& l) const noexcept
{
	auto dot = GetNormal()*l.GetNormal();
	return AlmostEqualRelativeAndAbs(dot, 0.f);
}

bool Luxko::Plane3DH::Perpendicular(const Vector3DH& v) const noexcept
{
	return v.Parallel(GetNormal());
}

Luxko::Point3DH Luxko::Plane3DH::Intersect(const Line3DH& l) const
{
	auto t = (_L.Dot(l.S.AsVector4f())) / (_L.Dot(l.Orientation().AsVector4f()));
	return l.S - t*l.Orientation();
}

Luxko::Line3DH Luxko::Plane3DH::Intersect(const Plane3DH& p) const
{
	Line3DH l;
	l.Orientation(GetNormal().Cross(p.GetNormal()));
	auto M = Matrix3x3f(A(), B(), C(),
		p.A(), p.B(), p.C(),
		l.Orientation().x(), l.Orientation().y(), l.Orientation().z());
	auto P = Vector3f(_L[3], p._L[3], 0.f);
	auto Q = M.Inverse()*P;
	l.S = Point3DH(Q);
	return l;
}

Luxko::Vector3DH Luxko::Plane3DH::GetNormal() const
{
	return Vector3DH(_L._data);
}
