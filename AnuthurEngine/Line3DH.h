#pragma once
#include "CommonHeader.h"
#include "Vector4f.h"
#include "Point3DH.h"
#include "Vector3DH.h"

namespace Luxko {
	class Plane3DH;

	class Line3DH {
	public:
		Line3DH() {}
		Line3DH(const Point3DH& p, const Vector3DH& v);
		Line3DH(const Line3DH&) = default;
		Line3DH& operator=(const Line3DH&) = default;
		~Line3DH() = default;

		bool operator==(const Line3DH& l)const;
		
		void InitializeN(const Point3DH& p, const Vector3DH& v);
		static Line3DH X() { return Line3DH(Point3DH::Origin(), Vector3DH::E1()); }
		static Line3DH Y() { return Line3DH(Point3DH::Origin(), Vector3DH::E2()); }
		static Line3DH Z() { return Line3DH(Point3DH::Origin(), Vector3DH::E3()); }

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

		
		const Vector3DH& Orientation()const noexcept { return V; }
		void Orientation(const Vector3DH& v);
		void OrientationN(const Vector3DH& vN);
		Point3DH S;
	private:
		Vector3DH V;
		
	};
}