//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Quaternionf.h"
#include "Transform3DH.h"

#include "FloatsComparision.h"

Luxko::Transform3DH Luxko::Transform3DH::New()
{
	return Transform3DH(Matrix4x4f::Identity(), true);
}

Luxko::Transform3DH Luxko::Transform3DH::RotationN(const Vector3DH& by, float theta)
{
	return Transform3DH(Matrix4x4f::RotationN(by.AsVector4f().xyz(), theta),true);
}

Luxko::Transform3DH Luxko::Transform3DH::Rotation(const Vector3DH& by, float theta)
{
	return RotationN(by.Normalize(), theta);
}

Luxko::Transform3DH Luxko::Transform3DH::Rotation(const Quaternionf& q)
{
	Transform3DH r;
	r._m = q.ToMatrix4x4f();
	r._subOrtho = true;
	return r;
}

Luxko::Transform3DH Luxko::Transform3DH::ScaleN(float by)
{
	return Transform3DH(Matrix4x4f(by, 0.f, 0.f, 0.f,
		0.f, by, 0.f, 0.f,
		0.f, 0.f, by, 0.f,
		0.f, 0.f, 0.f, 1.f),false);
}

Luxko::Transform3DH Luxko::Transform3DH::Scale(const Vector3DH& by)
{
	return Transform3DH(Matrix4x4f(by.x(), 0.f, 0.f, 0.f,
		0.f, by.y(), 0.f, 0.f,
		0.f, 0.f, by.z(), 0.f,
		0.f, 0.f, 0.f, 1.f), false);
}

Luxko::Transform3DH Luxko::Transform3DH::Scale(float xBy, float yBy, float zBy)
{
	return Transform3DH(Matrix4x4f(xBy, 0.f, 0.f, 0.f,
		0.f, yBy, 0.f, 0.f,
		0.f, 0.f, zBy, 0.f,
		0.f, 0.f, 0.f, 1.f), false);
}

Luxko::Transform3DH Luxko::Transform3DH::Translation(const Vector3DH& by)
{
	return Transform3DH(Matrix4x4f(1.f, 0.f, 0.f, by.x(),
		0.f, 1.f, 0.f, by.y(),
		0.f, 0.f, 1.f, by.z(),
		0.f, 0.f, 0.f, 1.f), true);
}

Luxko::Transform3DH Luxko::Transform3DH::Translation(float byX, float byY, float byZ)
{
	return Transform3DH(Matrix4x4f(1.f, 0.f, 0.f, byX,
		0.f, 1.f, 0.f, byY,
		0.f, 0.f, 1.f, byZ,
		0.f, 0.f, 0.f, 1.f), true);
}

Luxko::Transform3DH Luxko::Transform3DH::Projection(float n, float f, float l, float r, float b, float t)
{
	return Transform3DH(Matrix4x4f::Projection(n, f, l, r, b, t), false);
}

Luxko::Point3DH Luxko::Transform3DH::operator*(const Point3DH& v) const
{
	return Point3DH(_m*v.AsVector4f());
}

Luxko::Vector3DH Luxko::Transform3DH::operator*(const Vector3DH& v) const
{
	return Vector3DH(_m*v.AsVector4f());
}

Luxko::Transform3DH Luxko::Transform3DH::operator*(const Transform3DH& t) const
{
	return Transform3DH(_m*t._m, _subOrtho&&t._subOrtho);
}

Luxko::Transform3DH& Luxko::Transform3DH::operator*=(const Transform3DH& t)noexcept
{
	_m *= t._m;
	_subOrtho = _subOrtho&&t._subOrtho;
	return *this;
}

Luxko::Vector3DH Luxko::Transform3DH::ApplyOnNormal(const Vector3DH& v) const
{
	if (_subOrtho) {
		return this->operator*(v);
	}
	auto G = _m.GetUpperLeft3x3().Inverse().Transpose();
	auto V = Vector3f(v._data._data);
	V = G*V;
	return Vector3DH(V);
}

Luxko::Plane3DH Luxko::Transform3DH::ApplyOnPlane(const Plane3DH& p) const
{
	auto F = this->Inverse()._m.Transpose();
	return Plane3DH(F*p.AsVector4f());
}

Luxko::Transform3DH Luxko::Transform3DH::Inverse() const
{
	Transform3DH result;
	result._subOrtho = _subOrtho;
	if (_subOrtho) {
		result._m = _m.Transpose();
		auto m3x3 = result._m.GetUpperLeft3x3();
		auto T4 = result._m.Row(3);
		auto T3 = Vector3f(T4._data);
		auto m3x3_T3 = m3x3*(-T3);
		result._m._data[3] = m3x3_T3[0];
		result._m._data[7] = m3x3_T3[1];
		result._m._data[11] = m3x3_T3[2];
		result._m._vT[3] = { 0.f,0.f,0.f,1.f };
		return result;
	}
	result._m = _m.Inverse();
	return result;
}

Luxko::Transform3DH& Luxko::Transform3DH::InverseInPlace()
{
	Transform3DH& result = *this;
	result._subOrtho = _subOrtho;
	if (_subOrtho) {
		result._m = _m.Transpose();
		auto m3x3 = result._m.GetUpperLeft3x3();
		auto T4 = result._m.Row(3);
		auto T3 = Vector3f(T4._data);
		auto m3x3_T3 = m3x3*(-T3);
		result._m._data[3] = m3x3_T3[0];
		result._m._data[7] = m3x3_T3[1];
		result._m._data[11] = m3x3_T3[2];
		result._m._vT[3] = { 0.f,0.f,0.f,1.f };
		return result;
	}
	result._m = _m.Inverse();
	return result;
}

bool Luxko::Transform3DH::operator==(const Transform3DH& t) const
{
	return _m == t._m;
}
