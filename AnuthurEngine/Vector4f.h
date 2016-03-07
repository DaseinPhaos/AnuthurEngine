#pragma once
#include "CommonHeader.h"
#include "Vector3f.h"

namespace Luxko {
	class Vector4f {
	public:
		Vector4f(float a, float b, float c, float d) { x = a; y = b; z = c; w = d; }
		explicit Vector4f(const float* data);
		Vector4f(const Vector3f& v, float w) { x = v.x; y = v.y; z = v.z; this->w = w; }
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


		Vector4f Normalize()const;
		Vector4f Homogenerous()const;


		float Dot(const Vector4f& v)const;
		
		Vector3f xyz()const;
		Vector3f Projection()const;

		Matrix4x4f ToHomoMatrix()const;
		union {
			struct {
				float x, y, z, w;
			};
			float _data[4];
			__m128 m128;
		};
	};
	Vector4f operator*(float f, const Vector4f& v);
}
