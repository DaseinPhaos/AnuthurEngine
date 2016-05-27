//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "MathIO.h"
#include <iomanip>

std::ostream& operator<<(std::ostream& o, const Luxko::Vector3f& v) {
	auto flags = o.flags();

	o << "(" << std::setw(5) << v._x << ", " << v._y << ", " << v._z << ")" << std::flush;
	o.setf(flags);
	return o;
}

std::ostream& operator<<(std::ostream&o, const Luxko::Matrix3x3f& m) {
	auto flags = o.flags();
	o << std::setw(5) << "\r\n\t|" << m.At(0, 0) << ", " << m.At(0, 1) << ", " << m.At(0, 2)
		<< " |\r\n\t|" << m.At(1, 0) << ", " << m.At(1, 1) << ", " << m.At(1, 2)
		<< " |\r\n\t|" << m.At(2, 0) << ", " << m.At(2, 1) << ", " << m.At(2, 2) << " |\r\n" << std::flush;
	o.setf(flags);
	return o;
}

std::ostream& operator<<(std::ostream& o, const Luxko::Vector4f& v)
{
	auto flags = o.flags();

	o << "(" << std::setw(5) << v._x << ", " << v._y << ", " << v._z << ", " << v._w << ")" << std::flush;
	o.setf(flags);
	return o;
}

std::ostream& operator<<(std::ostream& o, const Luxko::Matrix4x4f& m)
{
	auto flags = o.flags();
	o << std::setw(5) << "\r\n\t|" << m.At(0, 0) << ", " << m.At(0, 1) << ", " << m.At(0, 2) << ", " << m.At(0, 3)
		<< " |\r\n\t|" << m.At(1, 0) << ", " << m.At(1, 1) << ", " << m.At(1, 2) << ", " << m.At(1, 3)
		<< " |\r\n\t|" << m.At(2, 0) << ", " << m.At(2, 1) << ", " << m.At(2, 2) << ", " << m.At(2, 3)
		<< " |\r\n\t|" << m.At(3, 0) << ", " << m.At(3, 1) << ", " << m.At(3, 2) << ", " << m.At(3, 3) << " |\r\n" << std::flush;
	o.setf(flags);
	return o;
}
