//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
//#ifdef ANUTHURMATH_EXPORTS
//#define ANUTHURMATH_API _declspec(dllexport)
//#else
//#define ANUTHURMATH_API _declspec(dllimport)
//#endif

#include "CommonHeader.h"
#include "Vector3f.h"
#include "OrientedObject.h"
#include "Line3DH.h"

namespace Luxko {
	class ANUTHURMATH_API Cylinder : public ConvexObject {
	public:
		Cylinder() {}
		Cylinder(float r, float s, float h,
			const Frame3DH& f = Frame3DH::StandardRightHandFrame())
			:_r(r), _s(s), _h(h), ConvexObject(f) {}
		Cylinder(const Vector3f& v, const Frame3DH& f = Frame3DH::StandardRightHandFrame())
			:_data(v), ConvexObject(f) {}
		Cylinder(const Cylinder&) = default;
		Cylinder& operator=(const Cylinder&) = default;

		virtual ~Cylinder() = default;

		virtual bool GetFirstIfIntersect(const Line3DH& line, Point3DH& at)const override;


		virtual bool Contain(const Point3DH& p)const override;

		

		// (x, y, z) should lie on the lateral surface.
		virtual Vector3DH GetNormalAt_ObjectSpace(float x, float y, float z)const override;

		
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