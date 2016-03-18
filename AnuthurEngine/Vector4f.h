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
#include "Vector3f.h"

namespace Luxko {
	class ANUTHURMATH_API Vector4f {
	public:
		Vector4f(float a, float b, float c, float d) { _x = a; _y = b; _z = c; _w = d; }
		explicit Vector4f(const float* data);
		Vector4f(const Vector3f& v, float w) { _x = v._x; _y = v._y; _z = v._z; this->_w = w; }
		Vector4f() = default;
		Vector4f(const Vector4f& v)noexcept;
		Vector4f& operator=(const Vector4f& v)noexcept;
		~Vector4f() = default;

		float& operator[](int i)noexcept { return _data[i]; }
		float operator[](int i)const noexcept { return _data[i]; }

		Vector4f operator+(const Vector4f& v)const;
		Vector4f operator-(const Vector4f& v)const;
		Vector4f operator-()const;
		Vector4f operator*(float s)const;
		Vector4f operator/(float s)const;

		Vector4f& operator+=(const Vector4f& v)noexcept;
		Vector4f& operator-=(const Vector4f& v)noexcept;
		Vector4f& operator*=(float f)noexcept;
		Vector4f& operator/=(float f)noexcept;

		bool operator==(const Vector4f& v)const;

		float Magnitude()const;
		float MagSquare()const;
		Vector4f& NormalizeInPlace()noexcept;
		Vector4f& HomogenizeInPlace()noexcept;


		Vector4f MemberWideMultiply(const Vector4f& v)const;
		Vector4f MemberWideMultiply(float x, float y, float z, float w)const;
		Vector4f Normalize()const;
		Vector4f Homogenerous()const;


		float Dot(const Vector4f& v)const;
		
		Vector3f xyz()const;
		Vector3f Projection()const;

		Matrix4x4f ToHomoMatrix()const;
		union {
			struct {
				float _x, _y, _z, _w;
			};
			float _data[4];
			__m128 _m128;
		};
	};
	ANUTHURMATH_API Vector4f operator*(float f, const Vector4f& v);
}
