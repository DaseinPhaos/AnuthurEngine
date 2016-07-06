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
#include "Vector4f.h"
#include "Point3DH.h"
#include "Vector3DH.h"

namespace Luxko {
	class Plane3DH;

	class ANUTHURMATH_API Line3DH {
	public:
		Line3DH() {}
		Line3DH(const Point3DH& p, const Vector3DH& v);
		Line3DH(const Line3DH&) = default;
		Line3DH& operator=(const Line3DH&) = default;
		~Line3DH() = default;

		bool operator==(const Line3DH& l)const;
		
		void InitializeN(const Point3DH& p, const Vector3DH& v);
		static Line3DH GetXAxis() { return Line3DH(Point3DH::Origin(), Vector3DH::E1()); }
		static Line3DH GetYAxis() { return Line3DH(Point3DH::Origin(), Vector3DH::E2()); }
		static Line3DH GetZAxis() { return Line3DH(Point3DH::Origin(), Vector3DH::E3()); }

		float Distance(const Point3DH& p)const noexcept;
		float Distance(const Line3DH& l)const noexcept;
		float Distance(const Plane3DH& p)const noexcept;
		bool Contain(const Point3DH& p)const noexcept;
		bool Parallel(const Vector3DH& v)const noexcept;
		bool Parallel(const Line3DH& l)const noexcept;
		bool Parallel(const Plane3DH& p)const noexcept;
		bool skew(const Line3DH& l)const noexcept;
		bool Perpendicular(const Vector3DH& v)const noexcept;
		bool Perpendicular(const Line3DH& l)const noexcept;
		bool Perpendicular(const Plane3DH& p)const noexcept;

		Point3DH operator()(float t)const;

		Point3DH Intersect(const Plane3DH& p)const;
		//Point3DH Intersect(const Line3DH& l)const;

		
		const Vector3DH& Orientation()const noexcept { return _V; }
		void Orientation(const Vector3DH& v);
		void OrientationN(const Vector3DH& vN);
		Point3DH _S;
	private:
		Vector3DH _V;
		
	};
}