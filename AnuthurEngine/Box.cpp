//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Box.h"
#include "FloatsComparision.h"

//Luxko::Box::RayBoxIntersectionResult Luxko::Box::Intersect(const Line3DH& line)const
//{
//	RayBoxIntersectionResult result;
//
//	auto ObjectSpaceTransform = GetObjectSpaceTransform();
//	auto lineOS = ObjectSpaceTransform.ApplyOnLine(line);
//
//
//}


bool Luxko::Box::FrontIntersect(const Line3DH& line, Point3DH& at) const
{
	auto ObjectSpaceTransform = GetObjectSpaceTransform();
	auto lineOS = ObjectSpaceTransform.ApplyOnLine(line);

	
	if (!AlmostEqualRelativeAndAbs(lineOS.Orientation().x(), 0.f)) {
		float rx = _width;
		if (lineOS.Orientation().x() > 0.f) {
			rx = 0.f;
		}
		auto t = (rx - lineOS._S.x()) / lineOS.Orientation().x();
		if (t < 0.f) {
			return false;
		}
		auto point = lineOS(t);
		auto py = point.y();
		auto pz = -point.z();
		if (py <= _height&&py >= 0.f
			&& pz<=_length&&pz>=0.f) {
			at = (ObjectSpaceTransform.Inverse())*point;
			return true;
		}
	}

	if (!AlmostEqualRelativeAndAbs(lineOS.Orientation().y(), 0.f)) {
		float ry = _height;
		if (lineOS.Orientation().y() > 0.f) {
			ry = 0.f;
		}
		auto t = (ry - lineOS._S.y()) / lineOS.Orientation().y();
		if (t < 0.f) {
			return false;
		}
		auto point = lineOS(t);
		auto px = point.x();
		auto pz = -point.z();
		if (px <= _width&&px >= 0.f
			&& pz <= _length&&pz >= 0.f) {
			at = (ObjectSpaceTransform.Inverse())*point;
			return true;
		}
	}

	if (!AlmostEqualRelativeAndAbs(lineOS.Orientation().z(), 0.f)) {
		float rz = -_length;
		if (lineOS.Orientation().z() < 0.f) {
			rz = 0.f;
		}
		auto t = (rz - lineOS._S.z()) / lineOS.Orientation().z();
		if (t < 0.f) {
			return false;
		}
		auto point = lineOS(t);
		auto px = point.x();
		auto py = point.y();
		if (py <= _height&&py >= 0.f
			&& px<=_width&&px>=0.f) {
			at = (ObjectSpaceTransform.Inverse())*point;
			return true;
		}
	}

	return false;
}
