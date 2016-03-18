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
#include "Vector4f.h"


//class Transformation3DH;

namespace Luxko {
	class Vector3DH;


	class ANUTHURMATH_API Point3DH {
	public:
		friend class Transform3DH;
		Point3DH() { _data._w = 1.f; }
		explicit Point3DH(const Vector3f& v);
		Point3DH(float x, float y, float z);
		explicit Point3DH(const float* data);
		Point3DH(const Point3DH& p) = default;
		Point3DH& operator=(const Point3DH& p) = default;
		~Point3DH() = default;

		static Point3DH Origin();
		static Point3DH E1();
		static Point3DH E2();
		static Point3DH E3();
		
		Point3DH operator+(const Vector3DH& v)const;
		Point3DH operator-(const Vector3DH& v)const;
		Point3DH operator-()const;
		Vector3DH operator-(const Point3DH& p)const;
		float operator*(const Point3DH& p)const;
		float operator*(const Vector3DH& v)const;
		Point3DH operator*(float f)const;
		Point3DH operator/(float f)const;

		Point3DH& operator+=(const Vector3DH& v)noexcept;
		Point3DH& operator-=(const Vector3DH& v)noexcept;
		Point3DH& operator*=(float f)noexcept;
		Point3DH& operator/=(float f)noexcept;

		bool operator==(const Point3DH& p)const { return _data == p._data; }

		Point3DH Cross(const Point3DH& p)const;
		Vector3f ToVector3f()const;
		Vector3DH ToVector3DH()const;
		float Length()const;
		Vector3DH Orientation()const;

		float& operator[](int i);
		float operator[](int i)const;
		float x()const { return _data._x; }
		float y()const{return _data._y;}
		float z()const{return _data._z;}
		const Vector4f& AsVector4f()const noexcept{ return _data; }
	private:
		Point3DH(const Vector4f& d);
		Vector4f _data;
	};
	Point3DH operator*(float f, const Point3DH& p);
}
