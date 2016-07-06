//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Matrix3x3f.h"
#include "Matrix4x4f.h"
#include <emmintrin.h>
#include "FloatsComparision.h"

Luxko::Matrix3x3f::Matrix3x3f(float x00, float x01, float x02, float x10, float x11, float x12, float x20, float x21, float x22)
{
	_data[0] = x00; _data[1] = x01; _data[2] = x02; _data[3] = x10; _data[4] = x11;
	_data[5] = x12; _data[6] = x20; _data[7] = x21; _data[8] = x22;
}

Luxko::Matrix3x3f::Matrix3x3f(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3)
{
	_data[0] = v1._x; _data[1] = v2._x; _data[2] = v3._x;
	_data[3] = v1._y; _data[4] = v2._y; _data[5] = v3._y;
	_data[6] = v1._z; _data[7] = v2._z; _data[8] = v3._z;
}

Luxko::Matrix3x3f::Matrix3x3f(const Vector3f* data)
{
	for (int i = 0; i < 3; ++i) {
		_data[i] = data[i]._x;
		_data[i + 3] = data[i]._y;
		_data[i + 6] = data[i]._z;
	}
}

Luxko::Matrix3x3f::Matrix3x3f(const float* data)
{
	for (int i = 0; i < 9; ++i) {
		_data[i] = data[i];
	}
}

Luxko::Matrix3x3f Luxko::Matrix3x3f::Zero()
{
	return Matrix3x3f(0.f, 0.f, 0.f,
		0.f, 0.f, 0.f,
		0.f, 0.f, 0.f);
}

Luxko::Matrix3x3f Luxko::Matrix3x3f::Identity()
{
	return Matrix3x3f(1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 0.f, 1.f);
}

Luxko::Matrix3x3f Luxko::Matrix3x3f::RotationN(const Vector3f& by, float theta)
{
	auto s = std::sinf(theta);
	auto c = std::cosf(theta);
	return Matrix3x3f(c + (1.f - c)*by._x*by._x, (1.f - c)*by._x*by._y - s*by._z, (1.f - c)*by._x*by._z + s*by._y,
		(1.f - c)*by._x*by._y + s*by._z, c + (1.f - c)*by._y*by._y, (1.f - c)*by._y*by._z - s*by._x,
		(1.f - c)*by._x*by._z - s*by._y, (1.f - c)*by._y*by._z + s*by._x, c + (1.f - c)*by._z*by._z);
}

Luxko::Matrix3x3f Luxko::Matrix3x3f::Rotation(const Vector3f& by, float theta)
{
	return RotationN(by.Normalize(), theta);
}

Luxko::Matrix3x3f Luxko::Matrix3x3f::ScaleN(float by)
{
	return Matrix3x3f(by, 0.f, 0.f,
		0.f, by, 0.f,
		0.f, 0.f, by);
}

Luxko::Matrix3x3f Luxko::Matrix3x3f::Scale(const Vector3f& by)
{
	return Matrix3x3f(by._x, 0.f, 0.f,
		0.f, by._y, 0.f,
		0.f, 0.f, by._z);
}

Luxko::Matrix3x3f Luxko::Matrix3x3f::Scale(float xBy, float yBy, float zBy)
{
	return Matrix3x3f(xBy, 0.f, 0.f,
		0.f, yBy, 0.f,
		0.f, 0.f, zBy);
}

float Luxko::Matrix3x3f::Determinant() const noexcept
{
	//auto mR2 = _mm_loadu_ps(_data + 3);
	//auto mR3 = _mm_loadu_ps(_data + 6);
	//auto p = _mm_mul_ps(_mm_shuffle_ps(mR2, mR2, _MM_SHUFFLE(0, 0, 2, 1)), /*[0,1,2,3]=[m11,m12,m10,m10] */
	//	_mm_shuffle_ps(mR3, mR3, _MM_SHUFFLE(0, 1, 0, 2)) /*[0,1,2,3]=[m22,m20,m21,m20]*/);
	//p = _mm_sub_ps(p,
	//	_mm_mul_ps(_mm_shuffle_ps(mR2, mR2, _MM_SHUFFLE(0, 1, 0, 2)),
	//		_mm_shuffle_ps(mR3, mR3, _MM_SHUFFLE(0, 0, 2, 1))));
	//float f[4];
	//_mm_storeu_ps(f, p);
	//return _data[0] * f[0] + _data[1] * f[1] + _data[2] * f[2];
	const float* m = _data;
	return m[0] * (m[4] * m[8] - m[5] * m[7]) + m[1] * (m[5] * m[6] - m[3] * m[8]) + m[2] * (m[3] * m[7] - m[4] * m[6]);
}

Luxko::Matrix3x3f Luxko::Matrix3x3f::Inverse() const
{
	//auto mR1 = _mm_loadu_ps(_data);
	//auto mR2 = _mm_loadu_ps(_data + 3);
	//auto mR3 = _mm_loadu_ps(_data + 6);
	//auto p = _mm_mul_ps(_mm_shuffle_ps(mR2, mR2, _MM_SHUFFLE(0, 0, 2, 1)), /*[0,1,2,3]=[m11,m12,m10,m10] */
	//	_mm_shuffle_ps(mR3, mR3, _MM_SHUFFLE(0, 1, 0, 2)) /*[0,1,2,3]=[m22,m20,m21,m20]*/);
	//p = _mm_sub_ps(p,
	//	_mm_mul_ps(_mm_shuffle_ps(mR2, mR2, _MM_SHUFFLE(0, 1, 0, 2)),
	//		_mm_shuffle_ps(mR3, mR3, _MM_SHUFFLE(0, 0, 2, 1))));
	//float f[10];
	//_mm_storeu_ps(f, p);
	//float det = _data[0] * f[0] + _data[1] * f[1] + _data[2] * f[2];

	//p = _mm_mul_ps(_mm_shuffle_ps(mR3, mR3, _MM_SHUFFLE(0, 0, 2, 1)), /*[0,1,2,3]=[m11,m12,m10,m10] */
	//	_mm_shuffle_ps(mR1, mR1, _MM_SHUFFLE(0, 1, 0, 2)) /*[0,1,2,3]=[m22,m20,m21,m20]*/);
	//p = _mm_sub_ps(p,
	//	_mm_mul_ps(_mm_shuffle_ps(mR3, mR3, _MM_SHUFFLE(0, 1, 0, 2)),
	//		_mm_shuffle_ps(mR1, mR1, _MM_SHUFFLE(0, 0, 2, 1))));
	//_mm_storeu_ps(f + 3, p);

	//p = _mm_mul_ps(_mm_shuffle_ps(mR1, mR1, _MM_SHUFFLE(0, 0, 2, 1)), /*[0,1,2,3]=[m11,m12,m10,m10] */
	//	_mm_shuffle_ps(mR2, mR2, _MM_SHUFFLE(0, 1, 0, 2)) /*[0,1,2,3]=[m22,m20,m21,m20]*/);
	//p = _mm_sub_ps(p,
	//	_mm_mul_ps(_mm_shuffle_ps(mR1, mR1, _MM_SHUFFLE(0, 1, 0, 2)),
	//		_mm_shuffle_ps(mR2, mR2, _MM_SHUFFLE(0, 0, 2, 1))));
	//_mm_storeu_ps(f + 6, p);

	//// Now f= { CT00, CT10, CT20, CT01, == p, later
	////			CT11, CT21, CT02, CT12, == mR1, later
	////			CT22 }
	//p = _mm_loadu_ps(f);
	//mR1 = _mm_loadu_ps(f + 4);
	//mR2 = _mm_load_ps1(&det);
	//p = _mm_div_ps(p, mR2);
	//_mm_storeu_ps(f, p);
	//mR1 = _mm_div_ps(mR1, mR2);
	//_mm_storeu_ps(f+4, mR1);
	////mR3 = _mm_shuffle_ps(p, mR1, _MM_SHUFFLE(0, 2, 3, 0)); // [0,1,2,3] = R00,R01,R02,R11,
	////													   // the last one should be set to R10
	////_mm_store_ps(f, mR3);
	////mR3 = _mm_shuffle_ps(mR1, p, _MM_SHUFFLE(0, 2, 3, 0)); // [0,1,2,3] = R11,R12,R20,R00,
	////													   // the last one should be set to R21
	////_mm_store_ps(f + 4, mR3);
	//
	//f[8] /= det;
	//return Matrix3x3f(f).Transpose();

	auto det = Determinant();
	const float* m = _data;
	Matrix3x3f r(m[4] * m[8] - m[5] * m[7], m[2] * m[7] - m[1] * m[8], m[1] * m[5] - m[2] * m[4],
		m[5] * m[6] - m[3] * m[8], m[0] * m[8] - m[2] * m[6], m[2] * m[3] - m[0] * m[5],
		m[3] * m[7] - m[4] * m[6], m[1] * m[6] - m[0] * m[7], m[0] * m[4] - m[1] * m[3]
		);
	return r / det;
}

Luxko::Matrix3x3f Luxko::Matrix3x3f::operator+(const Matrix3x3f& m) const
{

	Matrix3x3f result;
	auto p = _mm_loadu_ps(_data);
	auto q = _mm_loadu_ps(m._data);
	_mm_storeu_ps(result._data, _mm_add_ps(p, q));
	p = _mm_loadu_ps(_data + 4);
	q = _mm_loadu_ps(m._data + 4);
	_mm_storeu_ps(result._data + 4, _mm_add_ps(p, q));
	result._data[8] = _data[8] + m._data[8];
	return result;
}

Luxko::Matrix3x3f Luxko::Matrix3x3f::operator-(const Matrix3x3f& m) const
{
	Matrix3x3f result;
	auto p = _mm_loadu_ps(_data);
	auto q = _mm_loadu_ps(m._data);
	_mm_storeu_ps(result._data, _mm_sub_ps(p, q));
	p = _mm_loadu_ps(_data + 4);
	q = _mm_loadu_ps(m._data + 4);
	_mm_storeu_ps(result._data + 4, _mm_sub_ps(p, q));
	result._data[8] = _data[8] - m._data[8];
	return result;
}

Luxko::Matrix3x3f Luxko::Matrix3x3f::operator*(const Matrix3x3f& m) const
{
	float result[10];
	__m128 mRow[3];
	mRow[0] = _mm_loadu_ps(m._data);
	mRow[1] = _mm_loadu_ps(m._data + 3);
	mRow[2] = _mm_loadu_ps(m._data + 6);
	for (int i = 0; i < 3; ++i) {
		auto t = _mm_loadu_ps(_data + (i * 3));
		auto r = _mm_mul_ps(_mm_shuffle_ps(t, t, _MM_SHUFFLE(0, 0, 0, 0)), mRow[0]);
		r = _mm_add_ps(r,
			_mm_mul_ps(_mm_shuffle_ps(t, t, _MM_SHUFFLE(1, 1, 1, 1)), mRow[1]));
		r = _mm_add_ps(r,
			_mm_mul_ps(_mm_shuffle_ps(t, t, _MM_SHUFFLE(2, 2, 2, 2)), mRow[2]));
		_mm_storeu_ps(result + (i * 3), r);
	}
	return Matrix3x3f(result);
}

Luxko::Matrix3x3f& Luxko::Matrix3x3f::operator-=(const Matrix3x3f& m)noexcept
{
	for (int i = 0; i < 9; ++i) {
		_data[i] -= m._data[i];
	}
	return *this;
}

Luxko::Matrix3x3f& Luxko::Matrix3x3f::operator+=(const Matrix3x3f& m)noexcept
{
	for (int i = 0; i < 9; ++i) {
		_data[i] += m._data[i];
	}
	return *this;
}

Luxko::Matrix3x3f Luxko::Matrix3x3f::operator/(float x) const
{
	Matrix3x3f result;
	auto x128 = _mm_load_ps1(&x);
	auto m = _mm_loadu_ps(_data);
	m = _mm_div_ps(m, x128);
	_mm_storeu_ps(result._data, m);
	m = _mm_loadu_ps(_data + 4);
	m = _mm_div_ps(m, x128);
	_mm_storeu_ps(result._data + 4, m);
	result._data[8] = _data[8] / x;
	return result;
}

Luxko::Vector3f Luxko::Matrix3x3f::operator*(const Vector3f& v) const
{
	__m128 r[3];
	for (int i = 0; i < 3; ++i) {
		auto l = _mm_loadu_ps(Column(i)._data);
		auto vi = _mm_load_ps1(v._data + i);
		r[i] = _mm_mul_ps(l, vi);
	}
	r[0] = _mm_add_ps(r[0], r[1]);
	r[0] = _mm_add_ps(r[0], r[2]);
	float f[4];
	_mm_storeu_ps(f, r[0]);
	return Vector3f(f);
}

Luxko::Matrix3x3f& Luxko::Matrix3x3f::operator*=(const Matrix3x3f& m)noexcept
{
	float result[10];
	__m128 mRow[3];
	mRow[0] = _mm_loadu_ps(m._data);
	mRow[1] = _mm_loadu_ps(m._data + 3);
	mRow[2] = _mm_loadu_ps(m._data + 6);
	for (int i = 0; i < 3; ++i) {
		auto t = _mm_loadu_ps(_data + (i * 3));
		auto r = _mm_mul_ps(_mm_shuffle_ps(t, t, _MM_SHUFFLE(0, 0, 0, 0)), mRow[0]);
		r = _mm_add_ps(r,
			_mm_mul_ps(_mm_shuffle_ps(t, t, _MM_SHUFFLE(1, 1, 1, 1)), mRow[1]));
		r = _mm_add_ps(r,
			_mm_mul_ps(_mm_shuffle_ps(t, t, _MM_SHUFFLE(2, 2, 2, 2)), mRow[2]));
		_mm_storeu_ps(result + (i * 3), r);
	}
	for (int i = 0; i < 9; ++i) {
		_data[i] = result[i];
	}
	return *this;
}

Luxko::Matrix3x3f& Luxko::Matrix3x3f::operator*=(float f)noexcept
{
	for (int i = 0; i < 9; ++i) {
		_data[i] *= f;
	}
	return *this;
}

Luxko::Matrix3x3f& Luxko::Matrix3x3f::operator/=(float f)noexcept
{
	for (int i = 0; i < 9; ++i) {
		_data[i] /= f;
	}
	return *this;
}

bool Luxko::Matrix3x3f::operator==(const Matrix3x3f& m) const
{
	for (int i = 0; i < 9; ++i) {
		if (!AlmostEqualRelativeAndAbs(_data[i], m._data[i])) {
			return false;
		}
	}
	return true;
}

Luxko::Matrix3x3f Luxko::Matrix3x3f::Transpose() const
{
	return Matrix3x3f(_data[0], _data[3], _data[6],
		_data[1], _data[4], _data[7],
		_data[2], _data[5], _data[8]);
}

Luxko::Matrix3x3f Luxko::Matrix3x3f::operator*(float x) const
{
	Matrix3x3f result;
	auto x128 = _mm_load_ps1(&x);
	auto m = _mm_loadu_ps(_data);
	m = _mm_mul_ps(x128, m);
	_mm_storeu_ps(result._data, m);
	m = _mm_loadu_ps(_data + 4);
	m = _mm_mul_ps(x128, m);
	_mm_storeu_ps(result._data + 4, m);
	result._data[8] = x*_data[8];
	return result;
}

Luxko::Vector3f Luxko::Matrix3x3f::Row(int i) const
{
	return Vector3f(_data + (i * 3));
}

Luxko::Vector3f Luxko::Matrix3x3f::Column(int i) const
{
	return Vector3f(At(0, i), At(1, i), At(2, i));
}


Luxko::Matrix4x4f Luxko::Matrix3x3f::ToHomoMatrix4x4f() const
{
	return Matrix4x4f(_data[0], _data[1], _data[2], 0.f,
		_data[3], _data[4], _data[5], 0.f,
		_data[6], _data[7], _data[8], 0.f,
		0.f, 0.f, 0.f, 1.f);
}

Luxko::Matrix3x3f Luxko::Matrix3x3f::operator-() const
{
	Matrix3x3f result;
	for (int i = 0; i < 9; ++i) {
		result._data[i] = -_data[i];
	}
	return result;
}

Luxko::Matrix3x3f Luxko::operator*(float f, const Matrix3x3f& m)
{
	return m*f;
}
