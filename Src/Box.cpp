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


bool Luxko::Box::GetFirstIfIntersect(const Line3DH& line, Point3DH& at) const
{
	auto ObjectSpaceTransform = TransformWtoO();
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

bool Luxko::Box::Contain(const Point3DH& p) const
{
	return p.x() >= 0.f && p.x() <= _width
		&& p.y() >= 0.f && p.y() <= _height
		&& p.z() <= 0.f && p.z() >= -_length;

}

Luxko::Vector3DH Luxko::Box::GetNormalAt_ObjectSpace(float x, float y, float z) const
{
	if (AlmostEqualRelativeAndAbs(x, 0.f)) {
		return Vector3DH(-1.f, 0.f, 0.f);
	}
	if (AlmostEqualRelativeAndAbs(x, _width)) {
		return Vector3DH(1.f, 0.f, 0.f);
	}
	if (AlmostEqualRelativeAndAbs(y, 0.f)) {
		return Vector3DH(0.f, -1.f, 0.f);
	}
	if (AlmostEqualRelativeAndAbs(y, _height)) {
		return Vector3DH(0.f, 1.f, 0.f);
	}
	if (AlmostEqualRelativeAndAbs(z, 0.f)) {
		return Vector3DH(0.f, 0.f, 1.f);
	}
	if (AlmostEqualRelativeAndAbs(z, _length)) {
		return Vector3DH(0.f, 0.f, -1.f);
	}

	throw "Point ain't on box!";
}
