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
#include "Frame3DH.h"
#include "Transform3DH.h"

namespace Luxko {
	class ANUTHURMATH_API OrientedObject {
	public:
		OrientedObject() :_orientation(Frame3DH::StandardRightHandFrame()) {}
		OrientedObject(const Frame3DH& f) :_orientation(f) {}
		OrientedObject(const OrientedObject&) = default;
		OrientedObject& operator=(const OrientedObject&) = default;

		virtual ~OrientedObject(){}

		Point3DH ToObjectSpace(const Point3DH& p)const {
			auto OST = TransformWtoO();
			return OST*p;
		}
		Vector3DH ToObjectSpace(const Vector3DH& v)const {
			auto OST = TransformWtoO();
			return OST*v;
		}

		Point3DH GetPosition()const { return _orientation.Position(); }
		Vector3DH GetLookDirection()const { return _orientation.Look(); }
		Vector3DH GetRightDirection()const { return _orientation.Right(); }
		Vector3DH GetUpDirection()const { return _orientation.Up(); }
		Transform3DH TransformWtoO()const { return _orientation.GetTransform(); }
		Transform3DH TransformOtoW()const { return _orientation.GetTransform().Inverse(); }

		virtual void ApplyTransform(const Transform3DH& t) { _orientation = t.ApplyOnFrame(_orientation); }

		// Data members
		Frame3DH _orientation;
	};


	class ANUTHURMATH_API ConvexObject abstract:public OrientedObject {
	public:
		ConvexObject(){}
		ConvexObject(const Frame3DH& f) :OrientedObject(f) {}
		ConvexObject(const ConvexObject&) = default;
		ConvexObject& operator=(const ConvexObject&) = default;
		virtual ~ConvexObject() {}

		virtual bool GetFirstIfIntersect(const Line3DH& line, Point3DH& at)const = 0;

		virtual bool Contain(const Point3DH& p)const = 0;
		bool Contain(float x, float y, float z)const { return Contain(Point3DH(x, y, z)); }


		// (x, y, z) should lie on the lateral surface.
		virtual Vector3DH GetNormalAt_ObjectSpace(float x, float y, float z)const = 0;
		Vector3DH GetNormalAt_ObjectSpace(const Point3DH& p)const { return GetNormalAt_ObjectSpace(p.x(), p.y(), p.z()); }

		Vector3DH GetNormalAt(float x, float y, float z)const { return GetNormalAt(Point3DH(x, y, z)); }
		Vector3DH GetNormalAt(const Point3DH& p)const { return GetNormalAt_ObjectSpace(ToObjectSpace(p)); }

	};
}
