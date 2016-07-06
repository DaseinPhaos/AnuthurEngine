
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
#include "Point3DH.h"
#include "Line3DH.h"
#include "Plane3DH.h"

namespace Luxko {
	class ANUTHURMATH_API Triangle3DH {
	public:
		Triangle3DH() {}
		Triangle3DH(const Point3DH* points);
		Triangle3DH(const Point3DH& p1, const Point3DH& p2, const Point3DH& p3);
		Triangle3DH(const Triangle3DH&) = default;
		Triangle3DH& operator=(const Triangle3DH&) = default;
		~Triangle3DH() = default;

		Vector3DH GetNormal()const;
		Plane3DH GetPlane()const;

		bool Intersect(const Line3DH& line, Point3DH& at)const;
		bool IntersectBarycentric(const Line3DH& line, Vector3f& barycentricCoordinates)const;

		bool operator==(const Triangle3DH& t);
		const Point3DH& operator[](size_t index)const;
		Point3DH& operator[](size_t index);

		Point3DH _P0;
		Point3DH _P1;
		Point3DH _P2;
	};
}
