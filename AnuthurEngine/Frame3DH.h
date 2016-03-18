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
#include "Matrix3x3f.h"
#include "Vector3DH.h"
#include "Point3DH.h"
#include "Transform3DH.h"


namespace Luxko {
	class ANUTHURMATH_API Frame3DH {
	public:
		Frame3DH(const Vector3DH& look, const Vector3DH& up, const Point3DH& pos);
		static Frame3DH FromN(const Vector3DH& lookN, const Vector3DH& upN, const Point3DH& pos);
		Frame3DH(const Frame3DH& f) :_data(f._data) {}
		Frame3DH& operator=(const Frame3DH&);
		~Frame3DH() = default;

		static Frame3DH GetStandardRightHandFrame();

		const Vector3DH& Look()const { return _look; }
		const Vector3DH& Up()const { return _up; }
		const Vector3DH& Right()const { return _right; }
		const Point3DH& Position()const { return _Pos; }

		Transform3DH RightHandTransform()const;
		Transform3DH LeftHandTransform()const;

		
		Matrix3x3f ToCompressedM3x3()const;
	private:
		Frame3DH() {}
		union {
			struct {
				Vector3DH _up;
				Vector3DH _look;
				Vector3DH _right;
				Point3DH _Pos;
			};
			Matrix4x4f _data;
		};
	};
}