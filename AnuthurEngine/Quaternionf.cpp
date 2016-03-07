//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Quaternionf.h"
#include <emmintrin.h>
#include <cmath>

Luxko::Quaternionf::Quaternionf(float s, float vx, float vy, float vz) :_data(s, vx, vy, vz)
{
}

Luxko::Quaternionf::Quaternionf(const Vector4f& data) : _data(data)
{

}


Luxko::Quaternionf& Luxko::Quaternionf::Conjugatify()noexcept
{
	_data[1] = -_data[1];
	_data[2] = -_data[2];
	_data[3] = -_data[3];
	return *this;
}

Luxko::Quaternionf Luxko::Quaternionf::GetConjugate() const
{
	Quaternionf result;
	result._data[0] = _data[0];
	result._data[1] = -_data[1];
	result._data[2] = -_data[2];
	result._data[3] = -_data[3];
	return result;
}

Luxko::Quaternionf& Luxko::Quaternionf::InverseInPlace() noexcept
{
	auto magSq = _mm_set_ps1(_data.MagSquare());
	Conjugatify();
	_data.m128 = _mm_div_ps(_data.m128, magSq);
	return *this;

}

Luxko::Quaternionf Luxko::Quaternionf::Inverse() const
{
	auto magSq = _mm_set_ps1(_data.MagSquare());
	auto result = GetConjugate();
	result._data.m128 = _mm_div_ps(_data.m128, magSq);
	return result;
}


Luxko::Vector3f Luxko::Quaternionf::ApplyRotationOn(const Vector3f& v) const
{
	Quaternionf vQ(0.f, v.x, v.y, v.z);
	auto rQ = this->operator*(vQ)*Inverse();
	return Vector3f(rQ._data._data + 1);
}

Luxko::Vector4f Luxko::Quaternionf::ApplyRotationOn(const Vector4f& v) const
{
	Quaternionf vQ(0.f, v.x, v.y, v.z);
	auto rQ = this->operator*(vQ)*Inverse();
	return Vector4f(rQ._data.y, rQ._data.z, rQ._data.w, v.w);

}

Luxko::Vector3f Luxko::Quaternionf::ApplyRotationOnN(const Vector3f& v) const
{
	Quaternionf vQ(0.f, v.x, v.y, v.z);
	auto rQ = this->operator*(vQ)*GetConjugate();
	return Vector3f(rQ._data._data + 1);
}


Luxko::Vector4f Luxko::Quaternionf::ApplyRotationOnN(const Vector4f& v) const
{
	Quaternionf vQ(0.f, v.x, v.y, v.z);
	auto rQ = this->operator*(vQ)*GetConjugate();
	return Vector4f(rQ._data.y, rQ._data.z, rQ._data.w, v.w);
}

Luxko::Quaternionf Luxko::Quaternionf::FromRotationN(float theta, const Vector3f& normalizedAxis)
{
	theta /= 2.f;
	float s = std::sinf(theta);
	float c = std::cosf(theta);
	Quaternionf result;
	result._data.y = normalizedAxis.x*s;
	result._data.z = normalizedAxis.y*s;
	result._data.w = normalizedAxis.z*s;
	result._data.x = c;
	return result;
}

Luxko::Quaternionf Luxko::Quaternionf::FromRotationN(float theta, float nAxisX, float nAxisY, float nAxisZ)
{
	theta /= 2.f;
	float s = std::sinf(theta);
	float c = std::cosf(theta);
	Quaternionf result;
	result._data.y = nAxisX*s;
	result._data.z = nAxisY*s;
	result._data.w = nAxisZ*s;
	result._data.x = c;
	return result;
}

Luxko::Quaternionf Luxko::Quaternionf::FromRotation(float theta, const Vector3f& axis)
{
	return FromRotationN(theta, axis.Normalize());
}

Luxko::Quaternionf Luxko::Quaternionf::FromInterpolationLinear(float t, const Quaternionf& q1, const Quaternionf& q2)
{
	return Quaternionf((q1._data*t + q2._data*(1.f - t)).Normalize());
}

Luxko::Quaternionf Luxko::Quaternionf::FromInterpolationSpherical(float t, const Quaternionf& q1, const Quaternionf& q2)
{
	float theta = std::acosf(q1._data.Dot(q2._data));
	return Quaternionf((q1._data)*std::sinf(theta*t) + (q2._data)*std::sinf(theta*(1.f - t)) / std::sinf(theta));
}

Luxko::Quaternionf Luxko::Quaternionf::FromRotation(float theta, float axisX, float axisY, float axisZ)
{
	return FromRotationN(theta, Vector3f(axisX, axisY, axisZ).Normalize());
}

Luxko::Quaternionf Luxko::Quaternionf::operator*(const Quaternionf& q) const
{
	// *this = s1 + v1; q = s2 + v2
	auto a = _mm_load_ps1(&q._data.w); // a = (s2,s2,s2,s2)
	a = _mm_move_ss(a, q._data.m128); // a = (s1, s2, s2, s2)
	auto b = _mm_move_ss(_data.m128, q._data.m128); // b = (s2; v1)
	a = _mm_mul_ps(a, b); // a = (s1s2; s2v1)
	float dotP = _data._data[1] * q._data._data[1]
		+ _data._data[2] * q._data._data[2] + _data._data[3] * q._data._data[3];
	b = _mm_set_ps1(-dotP);
	b = _mm_move_ss(q._data.m128, b); // b = (-v1v2; v2)
	b = _mm_mul_ps(b, _mm_move_ss(_mm_load_ps1(_data._data), _mm_set_ss(1.f))); // b=(-v1v2; s2v2)
	a = _mm_add_ps(a, b);

	b = _mm_move_ss(_data.m128, _mm_setzero_ps()); // b=(0;v1);
	auto c = _mm_move_ss(q._data.m128, _mm_setzero_ps()); // c = (0;v2);
	auto r = _mm_mul_ps(_mm_shuffle_ps(b, b, _MM_SHUFFLE(1, 3, 2, 0)),
		_mm_shuffle_ps(c, c, _MM_SHUFFLE(2, 1, 3, 0)));
	r = _mm_sub_ps(r,
		_mm_mul_ps(_mm_shuffle_ps(b, b, _MM_SHUFFLE(2, 1, 3, 0)),
			_mm_shuffle_ps(c, c, _MM_SHUFFLE(1, 3, 2, 0)))); // r=(0;v1.cross(v2)
	a = _mm_add_ps(a, r);

	Quaternionf result;
	result._data.m128 = a;
	return result;
}

Luxko::Matrix3x3f Luxko::Quaternionf::ToMatrix3x3f() const
{
	Matrix3x3f result(1.f, -1.f, 1.f, 1.f, 1.f, -1.f, -1.f, 1.f, 1.f);
	auto two = _mm_set_ps1(2.f);
	auto a = _mm_mul_ps(_mm_shuffle_ps(_data.m128, _data.m128, _MM_SHUFFLE(1, 1, 1, 2)),
		_mm_shuffle_ps(_data.m128, _data.m128, _MM_SHUFFLE(1, 3, 1, 1)));
	auto b = _mm_div_ps(
		_mm_mul_ps(_mm_shuffle_ps(_data.m128, _data.m128, _MM_SHUFFLE(0, 0, 0, 3)),
			_mm_shuffle_ps(_data.m128, _data.m128, _MM_SHUFFLE(3, 2, 3, 3))),
		_mm_loadu_ps(result._data));
	_mm_storeu_ps(result._data, _mm_mul_ps(two, _mm_add_ps(a, b)));

	a = _mm_mul_ps(_mm_shuffle_ps(_data.m128, _data.m128, _MM_SHUFFLE(2, 1, 2, 1)),
		_mm_shuffle_ps(_data.m128, _data.m128, _MM_SHUFFLE(3, 3, 3, 1)));
	b = _mm_div_ps(
		_mm_mul_ps(_mm_shuffle_ps(_data.m128, _data.m128, _MM_SHUFFLE(0, 0, 0, 3)),
			_mm_shuffle_ps(_data.m128, _data.m128, _MM_SHUFFLE(1, 2, 1, 3))),
		_mm_loadu_ps(result._data + 4));
	_mm_storeu_ps(result._data + 4, _mm_mul_ps(two, _mm_add_ps(a, b)));

	result._data[0] = 1 - result._data[0];
	result._data[4] = 1 - result._data[4];
	result._data[8] = 1 - 2.f*_data._data[1] * _data._data[1] - 2.f*_data._data[2] * _data._data[2];
	return result;
}

Luxko::Matrix4x4f Luxko::Quaternionf::ToMatrix4x4f() const
{
	return ToMatrix3x3f().ToHomoMatrix4x4f();
}

Luxko::Quaternionf& Luxko::Quaternionf::operator*=(const Quaternionf& q)noexcept
{
	auto a = _mm_load_ps1(&q._data.w); // a = (s2,s2,s2,s2)
	a = _mm_move_ss(a, q._data.m128); // a = (s1, s2, s2, s2)
	auto b = _mm_move_ss(_data.m128, q._data.m128); // b = (s2; v1)
	a = _mm_mul_ps(a, b); // a = (s1s2; s2v1)
	float dotP = _data._data[1] * q._data._data[1]
		+ _data._data[2] * q._data._data[2] + _data._data[3] * q._data._data[3];
	b = _mm_set_ps1(-dotP);
	b = _mm_move_ss(q._data.m128, b); // b = (-v1v2; v2)
	b = _mm_mul_ps(b, _mm_move_ss(_mm_load_ps1(_data._data), _mm_set_ss(1.f))); // b=(-v1v2; s2v2)
	a = _mm_add_ps(a, b);

	b = _mm_move_ss(_data.m128, _mm_setzero_ps()); // b=(0;v1);
	auto c = _mm_move_ss(q._data.m128, _mm_setzero_ps()); // c = (0;v2);
	auto r = _mm_mul_ps(_mm_shuffle_ps(b, b, _MM_SHUFFLE(1, 3, 2, 0)),
		_mm_shuffle_ps(c, c, _MM_SHUFFLE(2, 1, 3, 0)));
	r = _mm_sub_ps(r,
		_mm_mul_ps(_mm_shuffle_ps(b, b, _MM_SHUFFLE(2, 1, 3, 0)),
			_mm_shuffle_ps(c, c, _MM_SHUFFLE(1, 3, 2, 0)))); // r=(0;v1.cross(v2)
	a = _mm_add_ps(a, r);
	_data.m128 = a;
	return *this;
}
