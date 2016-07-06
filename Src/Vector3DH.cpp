//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Vector3DH.h"
#include "Line3DH.h"
#include "Plane3DH.h"

Luxko::Vector3DH::Vector3DH(const Vector3f& v)
{
	_data._x = v._x;
	_data._y = v._y;
	_data._z = v._z;
	_data._w = 0.f;
}

Luxko::Vector3DH::Vector3DH(const Vector4f& d)
{
	_data = d;
}

Luxko::Vector3DH::Vector3DH(const float* data)
{
	_data._x = data[0];
	_data._y = data[1];
	_data._z = data[2];
	_data._w = 0.f;
}

Luxko::Vector3DH::Vector3DH(float x, float y, float z)
{
	_data._x = x;
	_data._y = y;
	_data._z = z;
	_data._w = 0.f;
}

Luxko::Vector3DH Luxko::Vector3DH::Zero()
{
	return Vector3DH(0.f, 0.f, 0.f);
}

Luxko::Vector3DH Luxko::Vector3DH::E1()
{
	return Vector3DH(1.f, 0.f, 0.f);
}

Luxko::Vector3DH Luxko::Vector3DH::E2()
{
	return Vector3DH(0.f, 1.f, 0.f);
}

Luxko::Vector3DH Luxko::Vector3DH::E3()
{
	return Vector3DH(0.f, 0.f, 1.f);
}

Luxko::Vector3DH Luxko::Vector3DH::operator-() const
{
	return Vector3DH(-_data);
}

Luxko::Point3DH Luxko::Vector3DH::operator+(const Point3DH& p) const
{
	return Point3DH((p.AsVector4f() + _data)._data);
}

Luxko::Vector3DH Luxko::Vector3DH::operator+(const Vector3DH& v) const
{
	return Vector3DH(_data + v._data);
}

Luxko::Vector3DH Luxko::Vector3DH::operator-(const Vector3DH& v) const
{
	return Vector3DH(_data - v._data);
}

float Luxko::Vector3DH::operator*(const Vector3DH& v) const
{
	return _data.Dot(v._data);
}

float Luxko::Vector3DH::operator*(const Point3DH& p) const
{
	return _data.Dot(p.AsVector4f());
}

Luxko::Vector3DH Luxko::Vector3DH::operator*(float f) const
{
	return Vector3DH(_data*f);
}

Luxko::Vector3DH Luxko::Vector3DH::operator/(float f) const
{
	return Vector3DH(_data / f);
}

Luxko::Vector3DH& Luxko::Vector3DH::operator+=(const Vector3DH& v) noexcept
{
	_data += v._data;
	return *this;
}

Luxko::Vector3DH& Luxko::Vector3DH::operator-=(const Vector3DH& v) noexcept
{
	_data -= v._data;
	return *this;
}

Luxko::Vector3DH& Luxko::Vector3DH::operator*=(float f) noexcept
{
	_data *= f;
	return *this;
}

Luxko::Vector3DH& Luxko::Vector3DH::operator/=(float f) noexcept
{
	_data /= f;
	return *this;
}

bool Luxko::Vector3DH::Parallel(const Vector3DH& v) const
{
	auto n = Normalize();
	return n == v.Normalize() || -n==v.Normalize();
}

bool Luxko::Vector3DH::Parallel(const Plane3DH& p) const
{
	return p.Parallel(*this);
}

bool Luxko::Vector3DH::Parallel(const Line3DH& v) const
{
	return v.Parallel(*this);
}

Luxko::Vector3DH Luxko::Vector3DH::Cross(const Vector3DH& p) const
{
	return Vector3DH(ToVector3f().Cross(p.ToVector3f()));
}

Luxko::Vector3f Luxko::Vector3DH::ToVector3f() const
{
	return Vector3f(_data._data);
}

float Luxko::Vector3DH::Length() const
{
	return _data.Magnitude();
}

Luxko::Vector3DH Luxko::Vector3DH::Normalize() const
{
	return Vector3DH(_data.Normalize());
}

float& Luxko::Vector3DH::operator[](int i)
{
	return _data[i];
}

float Luxko::Vector3DH::operator[](int i) const
{
	return _data[i];
}

Luxko::Vector3DH Luxko::operator*(float f, const Vector3DH& v)
{
	return v*f;
}
