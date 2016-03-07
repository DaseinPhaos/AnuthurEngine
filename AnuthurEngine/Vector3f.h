#pragma once
#include "CommonHeader.h"

namespace Luxko {
	class Vector4f;
	class Matrix3x3f;
	class Matrix4x4f;

	class Vector3f {
	public:
		Vector3f(float a, float b, float c) { x = a; y = b; z = c; }
		explicit Vector3f(const float* data) { x = data[0]; y = data[1]; z = data[2]; }
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


		float Dot(const Vector3f& v)const;
		Vector3f Cross(const Vector3f& v)const;
		Vector3f ProjectionAlong(const Vector3f v)const;
		Matrix3x3f ToCrossMatrix()const;
		Matrix3x3f ToHomoMatrix()const;

		union {
			struct {
				float x, y, z;
			};
			float _data[3];
		};

	private:

	};
	Vector3f operator*(float s, const Vector3f& v);
}