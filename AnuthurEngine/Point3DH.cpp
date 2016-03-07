//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Point3DH.h"
#include "Vector3DH.h"
//#include <exception>
Luxko::Point3DH::Point3DH(const Vector3f& v)
{
	_data.x = v.x;
	_data.y = v.y;
	_data.z = v.z;
	_data.w = 1.f;
}

Luxko::Point3DH::Point3DH(const float* data)
{
	_data.x = data[0];
	_data.y = data[1];
	_data.z = data[2];
	_data.w = 1.f;
}

Luxko::Point3DH::Point3DH(float x, float y, float z)
{
	_data.x = x;
	_data.y = y;
	_data.z = z;
	_data.w = 1.f;
}

Luxko::Point3DH::Point3DH(const Vector4f& d)
{
	_data = d;
}

Luxko::Point3DH Luxko::Point3DH::Origin()
{
	return Point3DH(0.f, 0.f, 0.f);
}

Luxko::Point3DH Luxko::Point3DH::E1()
{
	return Point3DH(1.f, 0.f, 0.f);
}

Luxko::Point3DH Luxko::Point3DH::E2()
{
	return Point3DH(0.f, 1.f, 0.f);
}

Luxko::Point3DH Luxko::Point3DH::E3()
{
	return Point3DH(0.f, 0.f, 1.f);
}

Luxko::Point3DH Luxko::Point3DH::operator+(const Vector3DH& v) const
{
	return Point3DH(AsVector4f()+v.AsVector4f());
}

Luxko::Point3DH Luxko::Point3DH::operator-() const
{
	return Point3DH(-_data._data[0], -_data._data[1], -_data._data[2]);
}

Luxko::Point3DH Luxko::Point3DH::operator-(const Vector3DH& v) const
{
	return Point3DH(AsVector4f()-v.AsVector4f());
}

Luxko::Vector3DH Luxko::Point3DH::operator-(const Point3DH& p) const
{
	return Vector3DH((AsVector4f() - p.AsVector4f())._data);
}

float Luxko::Point3DH::operator*(const Point3DH& p) const
{
	return _data.x*p._data.x + _data.y*p._data.y + _data.z*p._data.z;
}

Luxko::Point3DH Luxko::Point3DH::operator*(float f) const
{
	Point3DH result = Point3DH();
	for (int i = 0; i < 3; ++i) {
		result._data._data[i] = f*_data._data[i];
	}
	return result;
}

float Luxko::Point3DH::operator*(const Vector3DH& v) const
{
	return AsVector4f().Dot(v.AsVector4f());
}

Luxko::Point3DH Luxko::Point3DH::operator/(float f) const
{
	Point3DH p;
	for (int i = 0; i < 3; ++i) {
		p._data._data[i] /= f;
	}
	return p;
}

Luxko::Point3DH& Luxko::Point3DH::operator+=(const Vector3DH& v) noexcept
{
	_data += v.AsVector4f();
	return *this;
}

Luxko::Point3DH& Luxko::Point3DH::operator-=(const Vector3DH& v) noexcept
{
	_data -= v.AsVector4f();
	return *this;

}

Luxko::Point3DH& Luxko::Point3DH::operator*=(float f) noexcept
{
	for (int i = 0; i < 3; ++i) {
		_data._data[i] *= f;
	}
	return *this;
}

Luxko::Point3DH& Luxko::Point3DH::operator/=(float f) noexcept
{
	for (int i = 0; i < 3; ++i) {
		_data._data[i] /= f;
	}
	return *this;
}

Luxko::Point3DH Luxko::Point3DH::Cross(const Point3DH& p) const
{
	return Point3DH((*this).ToVector3f().Cross(p.ToVector3f()));
}

Luxko::Vector3f Luxko::Point3DH::ToVector3f() const
{
	return Vector3f(_data._data);
}

float Luxko::Point3DH::Length() const
{
	return ToVector3f().Magnitude();
}

Luxko::Vector3DH Luxko::Point3DH::Orientation() const
{
	return Vector3DH(ToVector3f().Normalize());
}

float& Luxko::Point3DH::operator[](int i)
{
	//if (i < 0 || i>3) {
	//	throw std::exception("Index out of bound.");
	//}
	return _data[i];
}

float Luxko::Point3DH::operator[](int i) const
{
	//if (i < 0 || i>3) {
	//	throw std::exception("Index out of bound.");
	//}
	return _data[i];
}

Luxko::Point3DH Luxko::operator*(float f, const Point3DH& p)
{
	return p*f;
}
