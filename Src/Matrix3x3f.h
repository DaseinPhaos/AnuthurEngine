//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
//#ifdef ANUTHURMATH_EXPORTS
//#define ANUTHURMATH_API _declspec(dllexport)
//#else
//#define ANUTHURMATH_API _declspec(dllimport)
//#endif
#include "CommonHeader.h"
#include "Vector3f.h"
namespace Luxko {
	class ANUTHURMATH_API Matrix3x3f {
	public:


		Matrix3x3f() {}
		explicit Matrix3x3f(const float* data);
		explicit Matrix3x3f(const Vector3f* data);
		Matrix3x3f(const Vector3f& v1, const Vector3f& v2, const Vector3f& v3);
		Matrix3x3f(float x00, float x01, float x02,
			float x10, float x11, float x12,
			float x20, float x21, float x22);
		Matrix3x3f(const Matrix3x3f&) = default;
		Matrix3x3f& operator=(const Matrix3x3f&) = default;
		~Matrix3x3f() = default;

		static Matrix3x3f Zero();
		static Matrix3x3f Identity();

		static Matrix3x3f RotationN(const Vector3f& by, float theta);
		static Matrix3x3f Rotation(const Vector3f& by, float theta);
		static Matrix3x3f ScaleN(float by);
		static Matrix3x3f Scale(const Vector3f& by);
		static Matrix3x3f Scale(float xBy, float yBy, float zBy);


		float Determinant()const noexcept;
		Matrix3x3f Inverse()const;
		Matrix3x3f operator+(const Matrix3x3f&)const;
		Matrix3x3f operator-(const Matrix3x3f&)const;
		Matrix3x3f operator-()const;
		Matrix3x3f operator*(float x)const;
		Matrix3x3f operator/(float x)const;
		Matrix3x3f operator*(const Matrix3x3f&)const;
		Vector3f operator*(const Vector3f&)const;

		Matrix3x3f& operator+=(const Matrix3x3f& m)noexcept;
		Matrix3x3f& operator-=(const Matrix3x3f& m)noexcept;
		Matrix3x3f& operator*=(const Matrix3x3f& m)noexcept;
		Matrix3x3f& operator*=(float f)noexcept;
		Matrix3x3f& operator/=(float f)noexcept;

		bool operator==(const Matrix3x3f& m)const;

		Matrix3x3f Transpose()const;
		Vector3f Row(int i)const;
		Vector3f Column(int i)const;
		float At(int c, int r)const noexcept { return _data[c * 3 + r]; }
		float& At(int c, int r)noexcept { return _data[c * 3 + r]; }

		Matrix4x4f ToHomoMatrix4x4f()const;
	
		float _data[9];
	};
	ANUTHURMATH_API Matrix3x3f operator*(float f, const Matrix3x3f& m);
}