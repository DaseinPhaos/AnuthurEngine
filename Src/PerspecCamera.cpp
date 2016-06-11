#include "PerspecCamera.h"

void Luxko::Anuthur::PerspecCamera::SetLens(float nearDistance, float farDistance, float aspectRatio, float hFov)
{
	_n = nearDistance;
	_f = farDistance;
	auto angle = hFov / 2.f;
	_r = nearDistance / std::tan(angle);
	_t = _r / aspectRatio;
}

Luxko::Transform3DH Luxko::Anuthur::PerspecCamera::TransformWtoV() const
{
	 return _orientation.GetTransformLH(); 
}

Luxko::Transform3DH Luxko::Anuthur::PerspecCamera::TransformVtoH() const
{
	return Transform3DH::HomoClipD3D(_n, _f, _r, _t);
}

Luxko::Transform3DH Luxko::Anuthur::PerspecCamera::TransformWtoH() const
{
	return TransformVtoH()*TransformWtoV();
}

Luxko::Anuthur::PerspecCamera Luxko::Anuthur::PerspecCamera::FromHFOVAndAspectRatio(float nearDistance, float farDistance, float aspectRatio, float angle, const Frame3DH& frame /*= Frame3DH::StandardRightHandFrame()*/)
{
	angle /= 2.f;
	auto r = nearDistance / std::tan(angle);
	auto t = r / aspectRatio;
	return PerspecCamera(nearDistance, farDistance, r, t, frame);
}

Luxko::Anuthur::PerspecCamera Luxko::Anuthur::PerspecCamera::FromHFOVAndAspectRatio(float nearDistance, float farDistance, float aspectRatio, float angle, const Vector3DH& lookDirection, const Vector3DH& upDirection, const Point3DH& position)
{
	angle /= 2.f;
	auto r = nearDistance / std::tan(angle);
	auto t = r / aspectRatio;
	return PerspecCamera(nearDistance, farDistance, r, t,
		lookDirection, upDirection, position);
}
