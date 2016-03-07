#pragma once
#include "CommonHeader.h"
#include "Vector3f.h"
#include "Vector4f.h"
#include "Point3DH.h"

namespace Luxko {
	class Line3DH;
	class Plane3DH;
	class Vector3DH {
	public:
		friend class Transform3DH;
		Vector3DH() { _data.w = 0.f; }
		explicit Vector3DH(const Vector3f& v);
		Vector3DH(float x, float y, float z);
		explicit Vector3DH(const float* data);
		Vector3DH(const Vector3DH& v) = default;
		Vector3DH& operator=(const Vector3DH& v) = default;
		~Vector3DH() = default;

		static Vector3DH Zero();
		static Vector3DH E1();
		static Vector3DH E2();
		static Vector3DH E3();

		Point3DH operator+(const Point3DH& p)const;
		Vector3DH operator+(const Vector3DH& v)const;
		Vector3DH operator-(const Vector3DH& v)const;
		Vector3DH operator-()const;
		float operator*(const Vector3DH& v)const;
		float operator*(const Point3DH& p)const;
		Vector3DH operator*(float f)const;
		Vector3DH operator/(float f)const;

		Vector3DH& operator+=(const Vector3DH& v)noexcept;
		Vector3DH& operator-=(const Vector3DH& v)noexcept;
		Vector3DH& operator*=(float f)noexcept;
		Vector3DH& operator/=(float f)noexcept;

		bool operator==(const Vector3DH& v)const { return _data == v._data; }
		bool Parallel(const Vector3DH& v)const;
		bool Parallel(const Line3DH& l)const;
		bool Parallel(const Plane3DH& p)const;

		Vector3DH Cross(const Vector3DH& p)const;
		Vector3f ToVector3f()const;
		float Length()const;
		Vector3DH Normalize()const;

		float& operator[](int i);
		float operator[](int i)const;
		float x()const { return _data.x; }
		float y()const { return _data.y; }
		float z()const { return _data.z; }
		const Vector4f& AsVector4f()const noexcept{ return _data; }

	private:
		Vector3DH(const Vector4f& d);
		Vector4f _data;

	};
	Vector3DH operator*(float f, const Vector3DH& v);
}