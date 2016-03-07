//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Vector3f.h"
#include "Matrix3x3f.h"
#include <emmintrin.h>
#include "FloatsComparision.h"
Luxko::Vector3f Luxko::Vector3f::operator+(const Vector3f& v) const
{
	//auto data = _mm_loadu_ps(_data);
	//auto vData = _mm_loadu_ps(v._data);
	//auto result = _mm_add_ps(data, vData);
	//float a[4];
	//_mm_storeu_ps(a, result);
	//return Vector3f(a);
//	return Vector3f(x + v.x, y + v.y, z + v.z);
	Vector3f sum;

	sum.x = x + v.x;
	sum.y = y + v.y;
	sum.z = z + v.z;

	return(sum);
}

Luxko::Vector3f Luxko::Vector3f::operator-(const Vector3f& v) const
{
	//auto data = _mm_loadu_ps(_data);
	//auto vData = _mm_loadu_ps(v._data);
	//auto result = _mm_sub_ps(data, vData);
	//float a[4];
	//_mm_storeu_ps(a, result);
	//return Vector3f(a);
	return Vector3f(x - v.x, y - v.y, z - v.z);
}

Luxko::Vector3f Luxko::Vector3f::operator*(float s) const
{
	//float by[4] = { s };
	//auto dBy = _mm_load1_ps(by);
	//auto data = _mm_loadu_ps(_data);
	//auto result = _mm_mul_ps(dBy, data);
	//_mm_storeu_ps(by, result);
	//return Vector3f(by);
	return Vector3f(x*s, y*s, z*s);
}



Luxko::Vector3f Luxko::Vector3f::operator/(float s) const
{
	//float by[4] = { s };
	//auto dBy = _mm_load1_ps(by);
	//auto data = _mm_loadu_ps(_data);
	//auto result = _mm_div_ps(data, dBy);
	//_mm_storeu_ps(by, result);
	//return Vector3f(by);
	return Vector3f(x / s, y / s, z / s);
}

Luxko::Vector3f& Luxko::Vector3f::operator-=(const Vector3f& v)noexcept
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Luxko::Vector3f& Luxko::Vector3f::operator+=(const Vector3f& v)noexcept
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Luxko::Vector3f& Luxko::Vector3f::operator*=(float f)noexcept
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

Luxko::Vector3f& Luxko::Vector3f::operator/=(float f)noexcept
{
	x /= f;
	y /= f;
	z /= f;
	return *this;
}

bool Luxko::Vector3f::operator==(const Vector3f& v) const
{
	for (int i = 0; i < 3; ++i) {
		if (!AlmostEqualRelativeAndAbs(_data[i], v._data[i])) {
			return false;
		}
	}
	return true;
}

float Luxko::Vector3f::Magnitude() const
{
	auto v = _mm_loadu_ps(_data);
	auto r = _mm_mul_ps(v, v);
	float f[4];
	_mm_storeu_ps(f, r);
	return sqrtf(f[0] + f[1] + f[2]);
}

float Luxko::Vector3f::MagSquare() const
{
	auto v = _mm_loadu_ps(_data);
	auto r = _mm_mul_ps(v, v);
	float f[4];
	_mm_storeu_ps(f, r);
	return f[0] + f[1] + f[2];
}

Luxko::Vector3f& Luxko::Vector3f::NormalizeInPlace() noexcept
{
	auto v = _mm_loadu_ps(_data);
	auto r = _mm_mul_ps(v, v);
	float f[4];
	_mm_storeu_ps(f, r);
	auto mag = sqrtf(f[0] + f[1] + f[2]);
	r = _mm_load1_ps(&mag);
	r = _mm_div_ps(v, r);
	_mm_storeu_ps(f, r);
	for (int i = 0; i < 3; ++i) {
		_data[i] = f[i];
	}
	return *this;
}

Luxko::Vector3f Luxko::Vector3f::Normalize() const
{
	auto v = _mm_loadu_ps(_data);
	auto r = _mm_mul_ps(v, v);
	float f[4];
	_mm_storeu_ps(f, r);
	auto mag = sqrtf(f[0] + f[1] + f[2]);
	r = _mm_load1_ps(&mag);
	r = _mm_div_ps(v, r);
	_mm_storeu_ps(f, r);
	return Vector3f(f);
}

float Luxko::Vector3f::Dot(const Vector3f& v) const
{
	//auto data = _mm_loadu_ps(_data);
	//auto vData = _mm_loadu_ps(v._data);
	//auto result = _mm_mul_ps(data, vData);
	//float a[4];
	//_mm_storeu_ps(a, result);
	//return a[0] + a[1] + a[2];
	return x*v.x + y*v.y + z*v.z;
}

Luxko::Vector3f Luxko::Vector3f::Cross(const Vector3f& v) const
{
	//auto p = _mm_loadu_ps(_data);
	////p4 = _mm_shuffle_ps(p4, p4, _MM_SHUFFLE(0,0,2,1));
	//auto q = _mm_loadu_ps(v._data);
	//auto qS = _mm_shuffle_ps(q, q, _MM_SHUFFLE(0,0,2,1));


	//auto r = _mm_mul_ps(p, qS); // r0 = pxqy, r1 = pyqz, r2 = pzqx

	//p = _mm_shuffle_ps(p, p, _MM_SHUFFLE(0,0,2,1)); // p0 = py, p1 = pz, p2 = px
	//p = _mm_mul_ps(p, q); // p0 = pyqx, p1 = pzqy, p2 = pxqz
	//r = _mm_sub_ps(r, p);
	//r = _mm_shuffle_ps(r, r, _MM_SHUFFLE(0,0,2,1));
	//float f[4];
	//_mm_storeu_ps(f, r);
	//return Vector3f(f);
	return Vector3f(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
}

Luxko::Vector3f Luxko::Vector3f::ProjectionAlong(const Vector3f v) const
{
	return v.Normalize()*(Dot(v));
}

Luxko::Matrix3x3f Luxko::Vector3f::ToCrossMatrix() const
{
	return Matrix3x3f(0.f, -z, y,
		z, 0.f, -x,
		-y, x, 0.f);
}

Luxko::Matrix3x3f Luxko::Vector3f::ToHomoMatrix() const
{
	return Matrix3x3f(x*x, x*y, x*z,
		x*y, y*y, y*z,
		x*z, y*z, z*z);
}

Luxko::Vector3f Luxko::Vector3f::operator-() const
{
	return Vector3f(-x, -y, -z);
}

Luxko::Vector3f Luxko::operator*(float s, const Vector3f& v)
{
	return v*s;
}

