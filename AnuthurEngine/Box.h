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
#include "Point3DH.h"
#include "Line3DH.h"
#include "ValResult.h"

namespace Luxko {
	class ANUTHURMATH_API Box : public OrientedObject {
	public:
		Box() {}
		Box(float width, float height, float length, const Frame3DH& f = Frame3DH::GetStandardRightHandFrame())
			:OrientedObject(f), _width(width), _height(height), _length(length) {}
		explicit Box(const Vector3f& data, const Frame3DH& f = Frame3DH::GetStandardRightHandFrame())
			:_data(data),OrientedObject(f) {}
		Box(const Box&) = default;
		Box& operator=(const Box&) = default;
		virtual ~Box() = default;

		// @Luxko: I'm currently not sure whether a full intersection method is necessary.
		//			Would be added if it turns out to be needed.
		//using RayBoxIntersectionResult = ValResult<float, 2U>;
		//RayBoxIntersectionResult Intersect(const Line3DH& line)const;

		bool GetFirstIfIntersect(const Line3DH& line, Point3DH& at)const;

		// Data members
		union {
			struct
			{
				float _width;
				float _height;
				float _length;
			};
			Vector3f _data;
		};
	};
}
