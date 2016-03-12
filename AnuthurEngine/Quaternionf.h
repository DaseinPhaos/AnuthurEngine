//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
#ifdef ANUTHURMATH_EXPORTS
#define ANUTHURMATH_API _declspec(dllexport)
#else
#define ANUTHURMATH_API _declspec(dllimport)
#endif
#include "CommonHeader.h"
#include "Vector4f.h"
#include "Vector3f.h"
#include "Matrix3x3f.h"
#include "Matrix4x4f.h"

namespace Luxko {
	class ANUTHURMATH_API Quaternionf {
	public:
		Quaternionf() {}
		Quaternionf(float s, float vx, float vy, float vz);
		explicit Quaternionf(const Vector4f& data);
		Quaternionf(const Quaternionf&) = default;
		Quaternionf& operator=(const Quaternionf&) = default;
		~Quaternionf() = default;
		Quaternionf& Conjugatify()noexcept;
		Quaternionf GetConjugate()const;
		Quaternionf& InverseInPlace()noexcept;
		Quaternionf Inverse()const;
		Vector3f ApplyRotationOn(const Vector3f& v)const;
		Vector4f ApplyRotationOn(const Vector4f& v)const; // Only apply to v's x,y,z
		Vector3f ApplyRotationOnN(const Vector3f& v)const;
		Vector4f ApplyRotationOnN(const Vector4f& v)const; // Only apply to v's x,y,z


		static Quaternionf FromRotationN(float theta, const Vector3f& normalizedAxis);
		static Quaternionf FromRotationN(float theta, float nAxisX, float nAxisY, float nAxisZ);
		static Quaternionf FromRotation(float theta, const Vector3f& axis);
		static Quaternionf FromRotation(float theta, float axisX, float axisY, float axisZ);
		static Quaternionf FromInterpolationLinear(float t, const Quaternionf& q1, const Quaternionf& q2);
		static Quaternionf FromInterpolationSpherical(float t, const Quaternionf& q1, const Quaternionf& q2);
		Quaternionf operator*(const Quaternionf& q)const;
		Quaternionf& operator*=(const Quaternionf& q)noexcept;

		bool operator==(const Quaternionf& q)const { return _data == q._data; }

		Matrix3x3f ToMatrix3x3f()const {
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
		Matrix4x4f ToMatrix4x4f()const { return ToMatrix3x3f().ToHomoMatrix4x4f(); }


		Vector4f _data;


	};
}