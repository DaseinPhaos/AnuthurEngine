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
	class ANUTHURMATH_API Cylinder : public OrientedObject {
	public:
		Cylinder() {}
		Cylinder(float r, float s, float h,
			const Frame3DH& f = Frame3DH::GetStandardRightHandFrame())
			:_r(r), _s(s), _h(h), OrientedObject(f) {}
		Cylinder(const Vector3f& v, const Frame3DH& f = Frame3DH::GetStandardRightHandFrame())
			:_data(v), OrientedObject(f) {}
		Cylinder(const Cylinder&) = default;
		Cylinder& operator=(const Cylinder&) = default;

		virtual ~Cylinder() = default;

		bool GetFirstIfIntersect(const Line3DH& line, Point3DH& at)const;
		
		bool Contain(const Point3DH& p)const;
		bool Contain(float x, float y, float z)const { return Contain(Point3DH(x, y, z)); }
		

		// (x, y, z) should lie on the lateral surface.
		Vector3DH GetNormalAt_ObjectSpace(float x, float y, float z)const;
		Vector3DH GetNormalAt_ObjectSpace(const Point3DH& p)const { return GetNormalAt_ObjectSpace(p.x(), p.y(), p.z()); }

		Vector3DH GetNormalAt(float x, float y, float z)const { return GetNormalAt(Point3DH(x, y, z)); }
		Vector3DH GetNormalAt(const Point3DH& p)const { return GetNormalAt_ObjectSpace(ToObjectSpace(p)); }

		
		// data members
		union {
			struct {
				float _r;
				float _s;
				float _h;
			};
			Vector3f _data;
		};

	};
}