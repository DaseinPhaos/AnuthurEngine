//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Matrix4x4f.h"
#include "FloatsComparision.h"
//#include <cstring>
Luxko::Matrix4x4f::Matrix4x4f(const float* data)
{
	for (int i = 0; i < 4; ++i) {
		m128[i] = _mm_loadu_ps(data + i);
	}
	//std::memcpy(_data, data, sizeof(Matrix4x4f));
}

Luxko::Matrix4x4f& Luxko::Matrix4x4f::operator=(const Matrix4x4f& m)noexcept
{
	for (int i = 0; i < 4; ++i) {
		_mm_storeu_ps(_data + i * 4, m.m128[i]);
	}
	/*std::memcpy(_data, m._data, sizeof(Matrix4x4f));*/
	return *this;
}

Luxko::Matrix4x4f::Matrix4x4f(const Matrix4x4f& m)noexcept
{
	for (int i = 0; i < 4; ++i) {
		_mm_storeu_ps(_data + i * 4, m.m128[i]);
	}
	/*std::memcpy(_data, m._data, sizeof(Matrix4x4f));*/
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::Zero()
{
	Matrix4x4f r;
	float f = 0.f;
	auto z = _mm_load_ps1(&f);
	for (int i = 0; i < 4; ++i) {
		_mm_storeu_ps(r._data + 4 * i, z);
	}
	return r;
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::Identity()
{
	Matrix4x4f r;
	float f = 0.f;
	auto z = _mm_load_ps1(&f);
	for (int i = 0; i < 4; ++i) {
		_mm_storeu_ps(r._data + 4 * i, z);
		r.At(i, i) = 1.f;
	}
	return r;
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::RotationN(const Vector3f& by, float theta)
{
	auto s = std::sinf(theta);
	auto c = std::cosf(theta);
	return Matrix4x4f(c + (1.f - c)*by.x*by.x, (1.f - c)*by.x*by.y - s*by.z, (1.f - c)*by.x*by.z + s*by.y, 0.f,
		(1.f - c)*by.x*by.y + s*by.z, c + (1.f - c)*by.y*by.y, (1.f - c)*by.y*by.z - s*by.x, 0.f,
		(1.f - c)*by.x*by.z - s*by.y, (1.f - c)*by.y*by.z + s*by.x, c + (1.f - c)*by.z*by.z, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::Rotation(const Vector3f& by, float theta)
{
	return RotationN(by.Normalize(), theta);
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::ScaleN(float by)
{
	return Matrix4x4f(by, 0.f, 0.f, 0.f,
		0.f, by, 0.f, 0.f,
		0.f, 0.f, by, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::Scale(const Vector3f& by)
{
	return Matrix4x4f(by.x, 0.f, 0.f, 0.f,
		0.f, by.y, 0.f, 0.f,
		0.f, 0.f, by.z, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::Scale(float xBy, float yBy, float zBy)
{
	return Matrix4x4f(xBy, 0.f, 0.f, 0.f,
		0.f, yBy, 0.f, 0.f,
		0.f, 0.f, zBy, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::Translation(const Vector3f& by)
{
	return Matrix4x4f(1.f, 0.f, 0.f, by.x,
		0.f, 1.f, 0.f, by.y,
		0.f, 0.f, 1.f, by.z,
		0.f, 0.f, 0.f, 1.f);
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::Translation(float byX, float byY, float byZ)
{
	return Matrix4x4f(1.f, 0.f, 0.f, byX,
		0.f, 1.f, 0.f, byY,
		0.f, 0.f, 1.f, byZ,
		0.f, 0.f, 0.f, 1.f);
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::Projection(float n, float f, float l, float r, float b, float t)
{
	Matrix4x4f result(2.f*n, 0.f, r + l, 0.f,
		0.f, 2.f*n, t + b, 0.f,
		0.f, 0.f, f + n, 2.f*n*f,
		0.f, 0.f, 1.f, 0.f);
	auto m_rml = _mm_set_ps1(l - r);
	result.m128[0] = _mm_div_ps(result.m128[0], m_rml);
	auto m_tmb = _mm_set_ps1(b - t);
	result.m128[1] = _mm_div_ps(result.m128[1], m_tmb);
	auto fmn = _mm_set_ps1(f - n);
	result.m128[2] = _mm_div_ps(result.m128[2], fmn);
	return result;
}

float Luxko::Matrix4x4f::Determinant() const noexcept
{
	auto a = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 0, 3))));
	auto b = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 1, 2, 2))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 0, 3))));
	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 1, 2, 2))));
	a = _mm_sub_ps(a, b);
	a = _mm_mul_ps(m128[0], a);
	return a.m128_f32[0] + a.m128_f32[1] + a.m128_f32[2] + a.m128_f32[3];
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::Inverse() const
{

	/*Matrix4x4f resultT;
	__m128 det;
	int x, y, z;
	for (int i = 0; i < 4; ++i) {
		if (i == 0) {
			x = 1; y = 2; z = 3;
		}
		else if (i == 1) {
			x = 0;
		}
		else if (i == 2) {
			y = 1;
		}
		else if (i == 3) {
			z = 2;
		}
		auto a = _mm_mul_ps(
			_mm_shuffle_ps(m128[x], m128[x], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_mul_ps(_mm_shuffle_ps(m128[y], m128[y], _MM_SHUFFLE(1, 1, 2, 2)),
				_mm_shuffle_ps(m128[z], m128[z], _MM_SHUFFLE(0, 3, 0, 3))));
		auto b = _mm_mul_ps(
			_mm_shuffle_ps(m128[x], m128[x], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_mul_ps(_mm_shuffle_ps(m128[y], m128[y], _MM_SHUFFLE(0, 3, 0, 3)),
				_mm_shuffle_ps(m128[z], m128[z], _MM_SHUFFLE(2, 0, 3, 1))));
		a = _mm_add_ps(a, b);
		b = _mm_mul_ps(
			_mm_shuffle_ps(m128[x], m128[x], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_mul_ps(_mm_shuffle_ps(m128[y], m128[y], _MM_SHUFFLE(2, 0, 3, 1)),
				_mm_shuffle_ps(m128[z], m128[z], _MM_SHUFFLE(1, 1, 2, 2))));
		a = _mm_add_ps(a, b);
		b = _mm_mul_ps(
			_mm_shuffle_ps(m128[x], m128[x], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_mul_ps(_mm_shuffle_ps(m128[y], m128[y], _MM_SHUFFLE(1, 1, 2, 2)),
				_mm_shuffle_ps(m128[z], m128[z], _MM_SHUFFLE(2, 0, 3, 1))));
		a = _mm_sub_ps(a, b);
		b = _mm_mul_ps(
			_mm_shuffle_ps(m128[x], m128[x], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_mul_ps(_mm_shuffle_ps(m128[y], m128[y], _MM_SHUFFLE(2, 0, 3, 1)),
				_mm_shuffle_ps(m128[z], m128[z], _MM_SHUFFLE(0, 3, 0, 3))));
		a = _mm_sub_ps(a, b);
		b = _mm_mul_ps(
			_mm_shuffle_ps(m128[x], m128[x], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_mul_ps(_mm_shuffle_ps(m128[y], m128[y], _MM_SHUFFLE(0, 3, 0, 3)),
				_mm_shuffle_ps(m128[z], m128[z], _MM_SHUFFLE(1, 1, 2, 2))));
		a = _mm_sub_ps(a, b);

		if (i == 0) {
			auto c = _mm_mul_ps(m128[0], a);
			float d = c.m128_f32[0] + c.m128_f32[1] + c.m128_f32[2] + c.m128_f32[3];
			det = _mm_load_ps1(&d);
		}
		if (i & 0x1 == 0x1) {
			float z = 0.f;
			auto zero = _mm_load_ps1(&z);
			a = _mm_sub_ps(zero, a);
		}
		resultT.m128[i] = _mm_div_ps(a, det);
	}
	return resultT.Transpose();*/

	//return InverseTranspose().Transpose();
	Matrix4x4f resultT;
	auto a = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 0, 3))));
	auto b = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 1, 2, 2))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 0, 3))));
	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 1, 2, 2))));
	a = _mm_sub_ps(a, b);
	auto c = _mm_mul_ps(m128[0], a);
	float d = c.m128_f32[0] + c.m128_f32[1] + c.m128_f32[2] + c.m128_f32[3];
	auto det = _mm_load_ps1(&d);
	resultT.m128[0] = _mm_div_ps(a, det);

	//**********************************************************************

	a = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 1, 2, 2))));
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 0, 3))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 0, 3))));

	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 1, 2, 2))));
	a = _mm_sub_ps(a, b);
	resultT.m128[1] = _mm_div_ps(a, det);

	//**********************************************************************

	a = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 0, 3))));
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 1, 2, 2))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 0, 3))));
	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 1, 2, 2))));
	a = _mm_sub_ps(a, b);
	resultT.m128[2] = _mm_div_ps(a, det);

	//**********************************************************************
	a = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 1, 2, 2))));
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 0, 3))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 0, 3))));

	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 1, 2, 2))));
	a = _mm_sub_ps(a, b);
	resultT.m128[3] = _mm_div_ps(a, det);
	return resultT.Transpose();
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::InverseTranspose() const
{
	Matrix4x4f resultT;
	auto a = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 0, 3))));
	auto b = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 1, 2, 2))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 0, 3))));
	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 1, 2, 2))));
	a = _mm_sub_ps(a, b);
	auto c = _mm_mul_ps(m128[0], a);
	float d = c.m128_f32[0] + c.m128_f32[1] + c.m128_f32[2] + c.m128_f32[3];
	auto det = _mm_load_ps1(&d);
	resultT.m128[0] = _mm_div_ps(a, det);

	//**********************************************************************

	a = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 1, 2, 2))));
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 0, 3))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 0, 3))));

	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 1, 2, 2))));
	a = _mm_sub_ps(a, b);
	resultT.m128[1] = _mm_div_ps(a, det);

	//**********************************************************************

	a = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 0, 3))));
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 1, 2, 2))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(0, 3, 0, 3))));
	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[3], m128[3], _MM_SHUFFLE(1, 1, 2, 2))));
	a = _mm_sub_ps(a, b);
	resultT.m128[2] = _mm_div_ps(a, det);

	//**********************************************************************
	a = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 1, 2, 2))));
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 0, 3))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_add_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(2, 0, 3, 1)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(1, 1, 2, 2)),
			_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(0, 3, 0, 3))));

	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(1, 1, 2, 2)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(0, 3, 0, 3)),
			_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(2, 0, 3, 1))));
	a = _mm_sub_ps(a, b);
	b = _mm_mul_ps(
		_mm_shuffle_ps(m128[0], m128[0], _MM_SHUFFLE(0, 3, 0, 3)),
		_mm_mul_ps(_mm_shuffle_ps(m128[1], m128[1], _MM_SHUFFLE(2, 0, 3, 1)),
			_mm_shuffle_ps(m128[2], m128[2], _MM_SHUFFLE(1, 1, 2, 2))));
	a = _mm_sub_ps(a, b);
	resultT.m128[3] = _mm_div_ps(a, det);
	return resultT;
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::operator+(const Matrix4x4f& m) const
{
	Matrix4x4f result;
	for (int i = 0; i < 4; ++i) {
		result.m128[i] = _mm_add_ps(m128[i], m.m128[i]);
	}
	return result;
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::operator-(const Matrix4x4f& m) const
{
	Matrix4x4f result;
	for (int i = 0; i < 4; ++i) {
		result.m128[i] = _mm_sub_ps(m128[i], m.m128[i]);
	}
	return result;
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::operator*(float x) const
{
	auto f = _mm_load_ps1(&x);
	Matrix4x4f result;
	for (int i = 0; i < 4; ++i) {
		result.m128[i] = _mm_mul_ps(m128[i], f);
	}
	return result;
}

bool Luxko::Matrix4x4f::operator==(const Matrix4x4f& m) const
{
	//auto td = std::fabsf(Determinant());
	//auto md = std::fabsf(m.Determinant());
	//auto d = max(td, md);
	//if (d < 1.f)
	//	d /= 4096;
	//else
	//	d = 1.f / 4096;
	for (int i = 0; i < 16; ++i) {
		if (!AlmostEqualRelativeAndAbs(_data[i], m._data[i]/*,d*/)) {
			return false;
		}
	}
	return true;
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::Transpose() const
{
	return Matrix4x4f(_vT);
}

Luxko::Vector4f Luxko::Matrix4x4f::Row(int i) const
{
	return _vT[i];
}

Luxko::Vector4f Luxko::Matrix4x4f::Column(int i) const
{
	return Vector4f(_vT[0][i], _vT[1][i], _vT[2][i], _vT[3][i]);
}

Luxko::Matrix3x3f Luxko::Matrix4x4f::GetUpperLeft3x3() const
{
	return Matrix3x3f(_data[0], _data[1], _data[2],
		_data[4], _data[5], _data[6],
		_data[8], _data[9], _data[10]);
}

Luxko::Matrix4x4f& Luxko::Matrix4x4f::operator/=(float x)noexcept
{
	auto f = _mm_load_ps1(&x);
	for (int i = 0; i < 4; ++i) {
		m128[i] = _mm_div_ps(m128[i], f);
	}
	return *this;
}

Luxko::Matrix4x4f& Luxko::Matrix4x4f::operator*=(float x)noexcept
{
	auto f = _mm_load_ps1(&x);
	for (int i = 0; i < 4; ++i) {
		m128[i] = _mm_mul_ps(m128[i], f);
	}
	return *this;
}

Luxko::Matrix4x4f& Luxko::Matrix4x4f::operator*=(const Matrix4x4f& m)noexcept
{
	for (int i = 0; i < 4; ++i) {
		auto a = _mm_shuffle_ps(m128[i], m128[i], _MM_SHUFFLE(0, 0, 0, 0));
		auto b = _mm_mul_ps(a, m.m128[0]);

		a = _mm_shuffle_ps(m128[i], m128[i], _MM_SHUFFLE(1, 1, 1, 1));
		b = _mm_add_ps(b, _mm_mul_ps(a, m.m128[1]));
		a = _mm_shuffle_ps(m128[i], m128[i], _MM_SHUFFLE(2, 2, 2, 2));
		b = _mm_add_ps(b, _mm_mul_ps(a, m.m128[2]));
		a = _mm_shuffle_ps(m128[i], m128[i], _MM_SHUFFLE(3, 3, 3, 3));
		m128[i] = _mm_add_ps(b, _mm_mul_ps(a, m.m128[3]));
	}
	return *this;
}

Luxko::Matrix4x4f& Luxko::Matrix4x4f::operator-=(const Matrix4x4f& m)noexcept
{
	for (int i = 0; i < 4; ++i) {
		m128[i] = _mm_sub_ps(m128[i], m.m128[i]);
	}
	return *this;
}

Luxko::Matrix4x4f& Luxko::Matrix4x4f::operator+=(const Matrix4x4f& m)noexcept
{
	for (int i = 0; i < 4; ++i) {
		m128[i] = _mm_add_ps(m128[i], m.m128[i]);
	}
	return *this;
}

Luxko::Vector4f Luxko::Matrix4x4f::operator*(const Vector4f& v) const
{
	Vector4f result;
	for (int i = 0; i < 4; ++i) {
		auto a = _mm_mul_ps(m128[i], v.m128);
		result[i] = a.m128_f32[0] + a.m128_f32[1] + a.m128_f32[2] + a.m128_f32[3];
	}
	return result;
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::operator*(const Matrix4x4f& m) const
{
	//Matrix4x4f mT = m.Transpose();
	Matrix4x4f result;
	for (int i = 0; i < 4; ++i) {
		auto a = _mm_shuffle_ps(m128[i], m128[i], _MM_SHUFFLE(0, 0, 0, 0));
		auto b = _mm_mul_ps(a, m.m128[0]);

		a = _mm_shuffle_ps(m128[i], m128[i], _MM_SHUFFLE(1, 1, 1, 1));
		b = _mm_add_ps(b, _mm_mul_ps(a, m.m128[1]));
		a = _mm_shuffle_ps(m128[i], m128[i], _MM_SHUFFLE(2, 2, 2, 2));
		b = _mm_add_ps(b, _mm_mul_ps(a, m.m128[2]));
		a = _mm_shuffle_ps(m128[i], m128[i], _MM_SHUFFLE(3, 3, 3, 3));
		result.m128[i] = _mm_add_ps(b, _mm_mul_ps(a, m.m128[3]));
	}
	return result;
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::operator/(float x) const
{
	auto f = _mm_load_ps1(&x);
	Matrix4x4f result;
	for (int i = 0; i < 4; ++i) {
		result.m128[i] = _mm_div_ps(m128[i], f);
	}
	return result;
}

Luxko::Matrix4x4f Luxko::Matrix4x4f::operator-() const
{
	Matrix4x4f result;
	for (int i = 0; i < 16; ++i) {
		result._data[i] = -_data[i];
	}
	return result;
}

Luxko::Matrix4x4f::Matrix4x4f(float x00, float x01, float x02, float x03,
	float x10, float x11, float x12, float x13,
	float x20, float x21, float x22, float x23,
	float x30, float x31, float x32, float x33)
{
	_data[0] = x00; _data[1] = x01; _data[2] = x02; _data[3] = x03;
	_data[4] = x10; _data[5] = x11; _data[6] = x12; _data[7] = x13;
	_data[8] = x20; _data[9] = x21; _data[10] = x22; _data[11] = x23;
	_data[12] = x30; _data[13] = x31; _data[14] = x32; _data[15] = x33;
}

Luxko::Matrix4x4f::Matrix4x4f(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3, const Vector4f& v4)
{
	for (int i = 0; i < 4; ++i) {
		_data[4 * i] = v1[i];
		_data[4 * i + 1] = v2[i];
		_data[4 * i + 2] = v3[i];
		_data[4 * i + 3] = v4[i];
	}
}

Luxko::Matrix4x4f::Matrix4x4f(const Vector4f* data)
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			_data[i + j * 4] = data[i]._data[j];
		}
	}
}

Luxko::Matrix4x4f Luxko::operator*(float f, const Matrix4x4f& m)
{
	return m*f;
}
