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
#include <ios>
#include "Vector3f.h"
#include "Matrix3x3f.h"
#include "Vector4f.h"
#include "Matrix4x4f.h"
#include "ValResult.h"

ANUTHURMATH_API std::ostream& operator<<(std::ostream& o, const Luxko::Vector3f& v);
ANUTHURMATH_API std::ostream& operator<<(std::ostream&o, const Luxko::Matrix3x3f& m);
ANUTHURMATH_API std::ostream& operator<<(std::ostream& o, const Luxko::Vector4f& v);
ANUTHURMATH_API std::ostream& operator<<(std::ostream& o, const Luxko::Matrix4x4f& m);

template<size_t N>
std::ostream& operator<<(std::ostream& o, const Luxko::ValResult<float, N>& vr);

template<size_t N>
std::ostream& operator<<(std::ostream& o, const Luxko::ValResult<float, N>& vr)
{
	o << "[size = " << vr.count << "\t";
	for (auto i = 0U; i < vr.count; ++i) {
		o << "#" << i << ": " << vr[i] << "\t";
	}
	o.flush();
	return o;
}
