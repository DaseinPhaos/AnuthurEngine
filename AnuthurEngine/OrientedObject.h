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
#include "Frame3DH.h"
#include "Transform3DH.h"

namespace Luxko {
	class ANUTHURMATH_API OrientedObject {
	public:
		OrientedObject() :_Orientation(Frame3DH::GetStandardRightHandFrame()) {}
		OrientedObject(const Frame3DH& f) :_Orientation(f) {}
		OrientedObject(const OrientedObject&) = default;
		OrientedObject& operator=(const OrientedObject&) = default;

		virtual ~OrientedObject(){}

		Point3DH ToObjectSpace(const Point3DH& p)const {
			auto OST = GetObjectSpaceTransform();
			return OST*p;
		}
		Vector3DH ToObjectSpace(const Vector3DH& v)const {
			auto OST = GetObjectSpaceTransform();
			return OST*v;
		}

		Point3DH GetPosition()const { return _Orientation.Position(); }
		Vector3DH GetLookDirection()const { return _Orientation.Look(); }
		Vector3DH GetRightDirection()const { return _Orientation.Right(); }
		Vector3DH GetUpDirection()const { return _Orientation.Up(); }
		Transform3DH GetObjectSpaceTransform()const { return _Orientation.RightHandTransform(); }
		
		virtual void ApplyTransform(const Transform3DH& t) { _Orientation = t.ApplyOnFrame(_Orientation); }

		// Data members
		Frame3DH _Orientation;
	};

}
