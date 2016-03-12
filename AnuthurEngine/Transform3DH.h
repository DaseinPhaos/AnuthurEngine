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
#include "Matrix4x4f.h"
#include "Point3DH.h"
#include "Vector3DH.h"
#include "Line3DH.h"
#include "Plane3DH.h"
#include "Quaternionf.h"

namespace Luxko {
	class ANUTHURMATH_API Transform3DH {
	public:
		Transform3DH(const Transform3DH&) = default;
		Transform3DH& operator=(const Transform3DH&) = default;
		~Transform3DH() = default;

		static Transform3DH New();
		static Transform3DH RotationN(const Vector3DH& by, float theta);
		static Transform3DH Rotation(const Vector3DH& by, float theta);
		static Transform3DH Rotation(const Quaternionf& q);
		static Transform3DH ScaleN(float by);
		static Transform3DH Scale(const Vector3DH& by);
		static Transform3DH Scale(float xBy, float yBy, float zBy);
		static Transform3DH Translation(const Vector3DH& by);
		static Transform3DH Translation(float byX, float byY, float byZ);
		static Transform3DH Projection(float n, float f, float l, float r, float b, float t);

		Transform3DH operator*(const Transform3DH& t)const;
		Transform3DH& operator*=(const Transform3DH& t)noexcept;
		Vector3DH operator*(const Vector3DH& v)const;
		Point3DH operator*(const Point3DH& v)const;
		Vector3DH ApplyOnNormal(const Vector3DH& v)const;
		Plane3DH ApplyOnPlane(const Plane3DH& p)const;

		bool operator==(const Transform3DH& t)const;

		Transform3DH Inverse()const;
		Transform3DH& InverseInPlace();

		const Matrix4x4f& AsMatrix4x4()const { return _m; }
		bool isSubMatrixOrthonormal()const { return _subOrtho; }

	private:
		Transform3DH() {}
		Transform3DH(const Matrix4x4f& m,bool isOrtho):_m(m),_subOrtho(isOrtho){}
		Matrix4x4f _m;
		bool _subOrtho;
	};
}