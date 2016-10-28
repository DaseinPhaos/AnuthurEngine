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

Luxko::Frame3DH Luxko::Frame3DH::StandardRightHandFrame()
{
	Frame3DH r;
	r._look = Vector3DH(0.f, 0.f, -1.f);
	r._right = Vector3DH(1.f, 0.f, 0.f);
	r._up = Vector3DH(0.f, 1.f, 0.f);
	r._Pos = Point3DH(0.f, 0.f, 0.f);
	return r;
}

Luxko::Frame3DH Luxko::Frame3DH::LookAt(const Point3DH& lookAt, const Vector3DH& up, const Point3DH& pos)
{
	auto lookTowards = lookAt - pos;
	return LookTowards(lookTowards, up, pos);
}

Luxko::Frame3DH Luxko::Frame3DH::LookTowards(const Vector3DH& lookTowards, const Vector3DH& up, const Point3DH& pos)
{
	return Frame3DH(lookTowards, up, pos);
}

void Luxko::Frame3DH::LookAt(const Point3DH & lookAt)
{
	auto lookTowards = lookAt - _Pos;
	LookTowards(lookTowards);
}

void Luxko::Frame3DH::LookTowards(const Vector3DH & lookTowards)
{
	assert(lookTowards != _up);
	_look = lookTowards.Normalize();
	_right = _look.Cross(_up)/*.Normalize()*/;
	_up = _right.Cross(_look);
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

Luxko::Transform3DH Luxko::Frame3DH::GetTransform() const
{
	Transform3DH rotation;
	rotation._m = Matrix4x4f(_right.AsVector4f(), _up.AsVector4f(),
		-_look.AsVector4f(), Vector4f(_Pos.x(), _Pos.y(), _Pos.z(), 1.f));
	rotation._subOrtho = true;
	return rotation.Inverse();
}

Luxko::Transform3DH Luxko::Frame3DH::GetTransformLH() const
{
	//Transform3DH rotation;
	//rotation._m = Matrix4x4f(_right.AsVector4f(), _up.AsVector4f(),
	//	_look.AsVector4f(), Vector4f(_Pos.x(), _Pos.y(), _Pos.z(), 1.f));
	//rotation._subOrtho = true;
	//return rotation.Inverse();

	Transform3DH result;
	result._m = _data.Transpose();
	result._subOrtho = true;
	return result.Inverse();
}

Luxko::Matrix3x3f Luxko::Frame3DH::ToCompressedM3x3() const
{
	return Matrix3x3f(_right.x(), _right.y(), _right.z(),
		_up.x(), _up.y(), _up.z(),
		_Pos.x(), _Pos.y(), _Pos.z());
}

void Luxko::Frame3DH::ApplyTransformOnOrientation(const Transform3DH& transform)
{
	_up = transform*_up;
	_look = transform*_look;
	_right = transform*_right;
}

void Luxko::Frame3DH::ApplyTransformOnPosition(const Transform3DH& transform)
{
	_Pos = transform*_Pos;
}

void Luxko::Frame3DH::ApplyTransform(const Transform3DH& transform)
{
	(*this) = transform.ApplyOnFrame(*this);
}
