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

		Matrix3x3f ToMatrix3x3f()const;
		Matrix4x4f ToMatrix4x4f()const;


		Vector4f _data;


	};
}