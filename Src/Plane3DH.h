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
#include "Point3DH.h"
#include "Vector3DH.h"
#include "Line3DH.h"
#include "Vector4f.h"

namespace Luxko {
	class ANUTHURMATH_API Plane3DH {
		friend class Transform3DH;
	public:
		Plane3DH() {}
		explicit Plane3DH(const Point3DH* data);
		Plane3DH(const Point3DH& p1, const Point3DH& p2, const Point3DH& p3);
		Plane3DH(const Point3DH& p, const Vector3DH& n);
		Plane3DH(const Plane3DH&) = default;
		Plane3DH& operator=(const Plane3DH&) = default;
		~Plane3DH() = default;
		void InitializeN(const Point3DH& p, const Vector3DH& normal);

		static Plane3DH XY() { return Plane3DH(Vector4f(0.f,0.f,1.f,0.f)); }
		static Plane3DH YZ() { return Plane3DH(Vector4f(1.f, 0.f, 0.f, 0.f)); }
		static Plane3DH ZX() { return Plane3DH(Vector4f(0.f, 1.f, 0.f, 0.f)); }
		
		Plane3DH operator-()const;

		float Distance(const Point3DH& p)const noexcept;
		float Distance(const Line3DH& l)const noexcept;
		float Distance(const Plane3DH& p)const noexcept;
		bool Contain(const Point3DH& p)const noexcept;
		bool Contain(const Line3DH& l)const noexcept;
		bool Parallel(const Vector3DH& v)const noexcept;
		bool Parallel(const Line3DH& l)const noexcept;
		bool Parallel(const Plane3DH& p)const noexcept;
		bool Perpendicular(const Vector3DH& v)const noexcept;
		bool Perpendicular(const Line3DH& l)const noexcept;
		bool Perpendicular(const Plane3DH& l)const noexcept;

		Point3DH Intersect(const Line3DH& l)const;
		Line3DH Intersect(const Plane3DH& p)const;

		bool operator==(const Plane3DH& p)const { return _L == p._L; }

		const Vector4f& AsVector4f()const noexcept { return _L; }
		Vector3DH GetNormal()const;
		float A()const noexcept { return _L[0]; }
		float B()const noexcept { return _L[1]; }
		float C()const noexcept { return _L[2]; }
		float D()const noexcept { return -_L[3]; }

	private:
		explicit Plane3DH(const Vector4f& v);
		Vector4f _L;
	};
}
