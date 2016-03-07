#pragma once
#include "CommonHeader.h"
#include <ios>
#include "Vector3f.h"
#include "Matrix3x3f.h"
#include "Vector4f.h"
#include "Matrix4x4f.h"
#include "ValResult.h"

std::ostream& operator<<(std::ostream& o, const Luxko::Vector3f& v);
std::ostream& operator<<(std::ostream&o, const Luxko::Matrix3x3f& m);
std::ostream& operator<<(std::ostream& o, const Luxko::Vector4f& v);
std::ostream& operator<<(std::ostream& o, const Luxko::Matrix4x4f& m);

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
