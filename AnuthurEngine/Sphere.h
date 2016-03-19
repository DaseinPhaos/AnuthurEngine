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
#include "OrientedObject.h"
#include "Line3DH.h"

namespace Luxko {
	class ANUTHURMATH_API Sphere : public OrientedObject {
	public:
		Sphere(){}
		Sphere(float r,const Frame3DH& f = Frame3DH::GetStandardRightHandFrame()):_r(r),OrientedObject(f) {}
		Sphere(const Sphere&) = default;
		Sphere& operator=(const Sphere&) = default;
		virtual ~Sphere() = default;


		bool GetFirstIfIntersect(const Line3DH& line, Point3DH& at)const;


		bool Contain(const Point3DH& p)const;
		bool Contain(float x, float y, float z)const { return Contain(Point3DH(x, y, z)); }


		// (x, y, z) should lie on the ellipsoid.
		Vector3DH GetNormalAt_ObjectSpace(float x, float y, float z)const { return Vector3DH(2.f*x, 2.f*y, 2.f*z); }
		Vector3DH GetNormalAt_ObjectSpace(const Point3DH& p)const { return GetNormalAt_ObjectSpace(p.x(), p.y(), p.z()); }

		Vector3DH GetNormalAt(float x, float y, float z)const { return GetNormalAt(Point3DH(x, y, z)); }
		Vector3DH GetNormalAt(const Point3DH& p)const { return GetNormalAt_ObjectSpace(ToObjectSpace(p)); }


		// Data members
		float _r;
	};
}