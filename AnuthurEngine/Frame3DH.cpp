//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Frame3DH.h"

Luxko::Frame3DH::Frame3DH(const Vector3DH& look, const Vector3DH& up, const Point3DH& pos)
{
	_look = look.Normalize();
	_right = look.Cross(up).Normalize();
	_up = _right.Cross(_look);
	_Pos = pos;

}

Luxko::Frame3DH& Luxko::Frame3DH::operator=(const Frame3DH& f)
{
	_data = f._data;
	return *this;
}

Luxko::Frame3DH Luxko::Frame3DH::FromN(const Vector3DH& lookN, const Vector3DH& upN, const Point3DH& pos)
{
	Frame3DH result;
	result._look = lookN;
	result._up = upN;
	result._right = lookN.Cross(upN);
	result._Pos = pos;
	return result;
}

Luxko::Transform3DH Luxko::Frame3DH::RightHandTransform() const
{
	auto translation = Transform3DH::Translation(_Pos.x(),_Pos.y(),_Pos.z());
	Transform3DH rotation;
	rotation._m = Matrix4x4f(_right.AsVector4f(), _up.AsVector4f(), -_look.AsVector4f(), Vector4f(0.f, 0.f, 0.f, 1.f));
	rotation._subOrtho = true;
	return (translation.Inverse())*(rotation*translation);

}

Luxko::Transform3DH Luxko::Frame3DH::LeftHandTransform() const
{
	auto translation = Transform3DH::Translation(_Pos.x(), _Pos.y(), _Pos.z());
	Transform3DH rotation;
	rotation._m = Matrix4x4f(_right.AsVector4f(), _up.AsVector4f(), _look.AsVector4f(), Vector4f(0.f, 0.f, 0.f, 1.f));
	rotation._subOrtho = true;
	return (translation.Inverse())*(rotation*translation);
}

Luxko::Matrix3x3f Luxko::Frame3DH::ToCompressedM3x3() const
{
	return Matrix3x3f(_right.x(), _right.y(), _right.z(),
		_up.x(), _up.y(), _up.z(),
		_Pos.x(), _Pos.y(), _Pos.z());
}
