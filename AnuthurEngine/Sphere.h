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


		bool FrontIntersect(const Line3DH& line, Point3DH& at)const;

		// Data members
		float _r;
	};
}