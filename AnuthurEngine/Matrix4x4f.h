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
//#include "Vector3f.h"
#include "Vector4f.h"
#include "Matrix3x3f.h"

namespace Luxko {
	class ANUTHURMATH_API Matrix4x4f {
	public:
		Matrix4x4f() {}
		explicit Matrix4x4f(const float* data);
		explicit Matrix4x4f(const Vector4f* data);
		Matrix4x4f(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3, const Vector4f& v4);
		Matrix4x4f(float x00, float x01, float x02, float x03,
			float x10, float x11, float x12, float x13,
			float x20, float x21, float x22, float x23,
			float x30, float x31, float x32, float x33);
		Matrix4x4f(const Matrix4x4f&)noexcept;
		Matrix4x4f& operator=(const Matrix4x4f&)noexcept;
		~Matrix4x4f() = default;

		static Matrix4x4f Zero();
		static Matrix4x4f Identity();
		static Matrix4x4f RotationN(const Vector3f& by, float theta);
		static Matrix4x4f Rotation(const Vector3f& by, float theta);
		static Matrix4x4f ScaleN(float by);
		static Matrix4x4f Scale(const Vector3f& by);
		static Matrix4x4f Scale(float xBy, float yBy, float zBy);
		static Matrix4x4f Translation(const Vector3f& by);
		static Matrix4x4f Translation(float byX, float byY, float byZ);
		static Matrix4x4f Projection(float n, float f, float l, float r, float b, float t);

		float Determinant()const noexcept;
		Matrix4x4f Inverse()const;
		Matrix4x4f InverseTranspose()const;
		Matrix4x4f operator+(const Matrix4x4f&)const;
		Matrix4x4f operator-(const Matrix4x4f&)const;
		Matrix4x4f operator-()const;
		Matrix4x4f operator*(float x)const;
		Matrix4x4f operator/(float x)const;
		Matrix4x4f operator*(const Matrix4x4f&)const;
		Vector4f operator*(const Vector4f&)const;

		Matrix4x4f& operator+=(const Matrix4x4f& m)noexcept;
		Matrix4x4f& operator-=(const Matrix4x4f& m)noexcept;
		Matrix4x4f& operator*=(const Matrix4x4f& m)noexcept;
		Matrix4x4f& operator*=(float f)noexcept;
		Matrix4x4f& operator/=(float f)noexcept;

		bool operator==(const Matrix4x4f& m)const;

		Matrix4x4f Transpose()const;
		Vector4f Row(int i)const;
		Vector4f Column(int i)const;
		float At(int c, int r)const noexcept { return _data[c * 4 + r]; }
		float& At(int c, int r)noexcept { return _data[c * 4 + r]; }

		Matrix3x3f GetUpperLeft3x3()const;


		union {
			float _data[16];
			Vector4f _vT[4];
			__m128 m128[4];
		};
	};
	ANUTHURMATH_API Matrix4x4f operator*(float f, const Matrix4x4f& m);
}