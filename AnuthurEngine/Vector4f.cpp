//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Vector4f.h"
#include "Matrix4x4f.h"
#include "FloatsComparision.h"


Luxko::Vector4f::Vector4f(const float* data)
{
	auto p = _mm_loadu_ps(data);
	_mm_storeu_ps(_data, p);
}

Luxko::Vector4f& Luxko::Vector4f::operator=(const Vector4f& v)noexcept
{
	_m128 = _mm_loadu_ps(v._data);
	return *this;
}

Luxko::Vector4f::Vector4f(const Vector4f& v)noexcept
{
	_m128 = _mm_loadu_ps(v._data);
}

Luxko::Vector4f Luxko::Vector4f::operator+(const Vector4f& v) const
{

	Vector4f result;
	result._m128 = _mm_add_ps(_m128, v._m128);
	return result;
}

Luxko::Vector4f Luxko::Vector4f::operator-(const Vector4f& v) const
{
	Vector4f result;
	result._m128 = _mm_sub_ps(_m128, v._m128);
	return result;
}

Luxko::Vector4f Luxko::Vector4f::operator*(float s) const
{
	auto r = _mm_load_ps1(&s);
	Vector4f result;
	result._m128 = _mm_mul_ps(r, _m128);
	return result;
}

Luxko::Vector4f Luxko::Vector4f::operator/(float s) const
{
	auto r = _mm_load_ps1(&s);
	Vector4f result;
	result._m128 = _mm_div_ps(_m128, r);
	return result;
}

Luxko::Vector4f Luxko::Vector4f::Modulate(const Vector4f& v) const
{
	Vector4f result;
	result._m128 = _mm_mul_ps(_m128, v._m128);
	return result;
}

Luxko::Vector4f& Luxko::Vector4f::operator-=(const Vector4f& v)noexcept
{
	_m128 = _mm_sub_ps(_m128, v._m128);
	return *this;
}

Luxko::Vector4f& Luxko::Vector4f::operator+=(const Vector4f& v)noexcept
{
	_m128 = _mm_add_ps(_m128, v._m128);
	return *this;
}

Luxko::Vector4f& Luxko::Vector4f::operator*=(float f)noexcept
{
	auto r = _mm_load_ps1(&f);
	_m128 = _mm_mul_ps(_m128, r);
	return *this;
}

Luxko::Vector4f& Luxko::Vector4f::operator/=(float f)noexcept
{
	auto r = _mm_load_ps1(&f);
	_m128 = _mm_div_ps(_m128, r);
	return *this;
}

bool Luxko::Vector4f::operator==(const Vector4f& v) const
{
	float absMax = 0.f;
	for (int i = 0; i < 4; ++i) {
		float k = std::fabs(_data[i]);
		float d = std::fabs(v[i]);
		auto m = max(k, d);
		if (absMax < m) {
			absMax = m;
		}
	}
	for (int i = 0; i < 4; ++i) {
		if (!AlmostEqualRelativeAndAbs(_data[i], v._data[i], absMax*_LUX_REL_EPSILON)) {
			return false;
		}
	}
	return true;
}

float Luxko::Vector4f::Magnitude() const
{
	auto r = _mm_mul_ps(_m128, _m128);
	return sqrtf(r.m128_f32[0] + r.m128_f32[1] + r.m128_f32[2] + r.m128_f32[3]);
}

float Luxko::Vector4f::MagSquare() const
{
	auto r = _mm_mul_ps(_m128, _m128);
	return r.m128_f32[0] + r.m128_f32[1] + r.m128_f32[2] + r.m128_f32[3];
}



Luxko::Vector4f Luxko::Vector4f::Clamp() const
{
	Vector4f result = (*this);
	for (auto i = 0; i < 4; ++i) {
		result._data[i] = (result._data[i] > 1.f) ? 1.f : result._data[i];
		result._data[i] = (result._data[i] < 0.f) ? 0.f : result._data[i];
	}
	return result;
}

Luxko::Vector4f& Luxko::Vector4f::ClampInPlace() noexcept
{
	for (auto i = 0; i < 4; ++i) {
		_data[i] = (_data[i] > 1.f) ? 1.f : _data[i];
		_data[i] = (_data[i] < 0.f) ? 0.f : _data[i];
	}
	return *this;
}

Luxko::Vector4f& Luxko::Vector4f::NormalizeInPlace() noexcept
{
	float mag = Magnitude();
	auto m = _mm_load_ps1(&mag);
	_m128 = _mm_div_ps(_m128, m);
	return *this;
}

Luxko::Vector4f& Luxko::Vector4f::HomogenizeInPlace() noexcept
{
	auto a = _mm_load_ps1(&_w);
	_m128 = _mm_div_ps(_m128, a);
	return *this;
}

Luxko::Vector4f Luxko::Vector4f::MemberWideMultiply(const Vector4f& v) const
{
	Vector4f result;
	result._m128 = _mm_mul_ps(_m128, v._m128);
	return result;
}

Luxko::Vector4f Luxko::Vector4f::MemberWideMultiply(float x, float y, float z, float w) const
{
	return Vector4f(x*_x, y*_y, z*_z, w*_w);
}

Luxko::Vector4f Luxko::Vector4f::Normalize() const
{
	Vector4f result;
	float mag = Magnitude();
	auto m = _mm_load_ps1(&mag);
	result._m128 = _mm_div_ps(_m128, m);
	return result;
}

Luxko::Vector4f Luxko::Vector4f::Homogenize() const
{
	Vector4f result;
	auto a = _mm_load_ps1(&_w);
	result._m128 = _mm_div_ps(_m128, a);
	return result;
}

float Luxko::Vector4f::Dot(const Vector4f& v) const
{
	auto r = _mm_mul_ps(_m128, v._m128);
	return r.m128_f32[0] + r.m128_f32[1] + r.m128_f32[2] + r.m128_f32[3];
}

Luxko::Vector3f Luxko::Vector4f::xyz() const
{
	return Vector3f(_x, _y, _z);
}

Luxko::Vector3f Luxko::Vector4f::Projection() const
{
	return Vector3f(_x / _w, _y / _w, _z / _w);
}

Luxko::Matrix4x4f Luxko::Vector4f::ToHomoMatrix() const
{
	Matrix4x4f result;
	for (int i = 0; i < 4; ++i) {
		auto a = _mm_load_ps1(_data + i);
		result._m128[i] = _mm_mul_ps(a, _m128);
	}
	return result;
}

Luxko::Vector4f Luxko::Vector4f::operator-() const
{
	return Vector4f(-_x, -_y, -_z, -_w);
}

Luxko::Vector4f Luxko::operator*(float f, const Vector4f& v)
{
	return v*f;
}


