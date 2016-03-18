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

namespace Luxko {
	class Vector4f;
	class Matrix3x3f;
	class Matrix4x4f;

	class ANUTHURMATH_API Vector3f {
	public:
		Vector3f(float a, float b, float c) { _x = a; _y = b; _z = c; }
		explicit Vector3f(const float* data) { _x = data[0]; _y = data[1]; _z = data[2]; }
		Vector3f() {}
		Vector3f(const Vector3f& v)/* { x = v.x; y = v.y;z=v.z; }*/=default;
		Vector3f& operator=(const Vector3f& v)/* { x = v.x; y = v.y; z = v.z; return *this; }*/=default;
		~Vector3f() = default;

		float& operator[](int i)noexcept { return _data[i]; }
		float operator[](int i)const noexcept { return _data[i]; }

		Vector3f operator+(const Vector3f& v)const;
		Vector3f operator-(const Vector3f& v)const;
		Vector3f operator-()const;
		Vector3f operator*(float s)const;
		Vector3f operator/(float s)const;

		Vector3f& operator+=(const Vector3f& v)noexcept;
		Vector3f& operator-=(const Vector3f& v)noexcept;
		Vector3f& operator*=(float f)noexcept;
		Vector3f& operator/=(float f)noexcept;

		bool operator==(const Vector3f& v)const;

		float Magnitude()const;
		float MagSquare()const;
		Vector3f& NormalizeInPlace()noexcept;
		Vector3f Normalize()const;

		Vector3f ElementWideMultiply(const Vector3f& v)const;
		Vector3f ElementWideMultiply(float byX, float byY, float byZ)const;
		float Dot(const Vector3f& v)const;
		Vector3f Cross(const Vector3f& v)const;
		Vector3f ProjectionAlong(const Vector3f v)const;
		Matrix3x3f ToCrossMatrix()const;
		Matrix3x3f ToHomoMatrix()const;

		union {
			struct {
				float _x, _y, _z;
			};
			float _data[3];
		};

	private:

	};
	ANUTHURMATH_API Vector3f operator*(float s, const Vector3f& v);
}