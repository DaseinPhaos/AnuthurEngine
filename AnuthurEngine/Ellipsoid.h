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
#include "Sphere.h"
namespace Luxko {
	class ANUTHURMATH_API Ellipsoid :public OrientedObject {
	public:
		Ellipsoid() {}
		Ellipsoid(const Sphere& s) :OrientedObject(s._Orientation),
			_m(1.f), _n(1.f), _r(s._r) {}
		Ellipsoid(float r, float m = 0.f, float n = 0.f, 
			const Frame3DH& f = Frame3DH::GetStandardRightHandFrame())
			:_m(m), _n(n), _r(r),OrientedObject(f) {}
		Ellipsoid(const Vector3f& v, const Frame3DH& f = Frame3DH::GetStandardRightHandFrame())
			:_data(v),OrientedObject(f) {}
		Ellipsoid(const Ellipsoid&) = default;
		Ellipsoid& operator=(const Ellipsoid&) = default;
		
		virtual ~Ellipsoid() = default;

		bool GetFirstIfIntersect(const Line3DH& line, Point3DH& at)const;
		
		bool Contain(const Point3DH& p)const;
		bool Contain(float x, float y, float z)const { return Contain(Point3DH(x, y, z)); }


		// (x, y, z) should lie on the ellipsoid.
		Vector3DH GetNormalAt_ObjectSpace(float x, float y, float z)const{ return Vector3DH(2.f*x, 2.f*_m*_m*y, 2.f*_n*_n*z); }
		Vector3DH GetNormalAt_ObjectSpace(const Point3DH& p)const { return GetNormalAt_ObjectSpace(p.x(), p.y(), p.z()); }

		Vector3DH GetNormalAt(float x, float y, float z)const { return GetNormalAt(Point3DH(x, y, z)); }
		Vector3DH GetNormalAt(const Point3DH& p)const { return GetNormalAt_ObjectSpace(ToObjectSpace(p)); }

		// data members
		union {
			struct {
				float _m;
				float _n;
				float _r;
			};
			Vector3f _data;
		};
	};
}