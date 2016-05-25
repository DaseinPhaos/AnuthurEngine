//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#include "LightandMaterial.h"

Luxko::Anuthur::PointLightLinear::PointLightLinear(const Point3DH& position, const ColorRGBf& strength, float falloffStart, float fallOffEnd)
{
	_irradiance = strength;
	_position = position.AsVector4f().xyz();
	_falloffStartDistance = falloffStart;
	_falloffEndDistance = fallOffEnd;
}

Luxko::Anuthur::ColorRGBf Luxko::Anuthur::PointLightLinear::GetIrradianceAtWorldCoor(const Point3DH& p) const
{
	auto p4 = Point3DH(_position);
	auto distanceVec = p4 - p;
	auto distance = distanceVec.Length();
	float delta;
	if (distance >= _falloffEndDistance) delta = 0.f;
	else if (distance <= _falloffStartDistance) delta = 1.f;
	else delta =1.f - (distance - _falloffStartDistance) / (_falloffEndDistance - _falloffStartDistance);
	return delta * _irradiance;

}

Luxko::Vector3DH Luxko::Anuthur::PointLightLinear::GetLightVectorAtWorldCoor(const Point3DH& p) const
{
	auto p4 = Point3DH(_position);
	auto distanceVec = p4 - p;
	return distanceVec.Normalize();
}

std::pair<Luxko::Vector3DH, Luxko::Anuthur::ColorRGBf> Luxko::Anuthur::PointLightLinear::GetAttributesAtWorldCoor(const Point3DH& p) const
{
	auto p4 = Point3DH(_position);
	auto distanceVec = p4 - p;
	auto distance = distanceVec.Length();
	float delta;
	if (distance >= _falloffEndDistance) delta = 0.f;
	else if (distance <= _falloffStartDistance) delta = 1.f;
	else delta = 1.f - (distance - _falloffStartDistance) / (_falloffEndDistance - _falloffStartDistance);
	return{ distanceVec.Normalize(), delta * _irradiance };
}

Luxko::Anuthur::PointLightQuadra::PointLightQuadra(const Point3DH& position, const ColorRGBf& strength, float falloffCoefficient)
{
	_position = position;
	_irradiance = strength;
	_falloffCoefficient = falloffCoefficient;
}

Luxko::Anuthur::ColorRGBf Luxko::Anuthur::PointLightQuadra::GetIrradianceAtWorldCoor(const Point3DH& p) const
{
	auto distanceVec = _position - p;
	auto distance = distanceVec.Length();
	auto delta = _falloffCoefficient * distance + 1.f;
	delta *= delta;
	return delta * _irradiance;
}

Luxko::Vector3DH Luxko::Anuthur::PointLightQuadra::GetLightVectorAtWorldCoor(const Point3DH& p) const
{
	auto distanceVec = _position - p;
	return distanceVec.Normalize();
}

std::pair<Luxko::Vector3DH, Luxko::Anuthur::ColorRGBf> Luxko::Anuthur::PointLightQuadra::GetAttributesAtWorldCoor(const Point3DH& p) const
{
	auto distanceVec = _position - p;
	auto distance = distanceVec.Length();
	auto delta = _falloffCoefficient * distance + 1.f;
	delta *= delta;
	return{ distanceVec.Normalize(), delta * _irradiance };
}

Luxko::Anuthur::DirectionalLight::DirectionalLight(const Vector3DH& direction, const ColorRGBf& strength)
{
	_direction = direction;
	_irradiance = strength;
}

Luxko::Anuthur::ColorRGBf Luxko::Anuthur::DirectionalLight::GetIrradianceAtWorldCoor(const Point3DH& p) const
{
	return _irradiance;
}

Luxko::Vector3DH Luxko::Anuthur::DirectionalLight::GetLightVectorAtWorldCoor(const Point3DH& p) const
{
	return _direction;
}

std::pair<Luxko::Vector3DH, Luxko::Anuthur::ColorRGBf> Luxko::Anuthur::DirectionalLight::GetAttributesAtWorldCoor(const Point3DH& p) const
{
	return{ _direction, _irradiance };
}

Luxko::Anuthur::SpotLightLinear::SpotLightLinear(const Point3DH& position, const ColorRGBf& strength, float falloffStart, float falloffEnd, const Vector3DH& direction, float power /*= 8.f*/)
{
	_position = position.AsVector4f().xyz();
	_irradiance = strength;
	_falloffStartDistance = falloffStart;
	_falloffEndDistance = falloffEnd;
	_direction = direction.AsVector4f().xyz();
	_power = power;
}

Luxko::Anuthur::ColorRGBf Luxko::Anuthur::SpotLightLinear::GetIrradianceAtWorldCoor(const Point3DH& p) const
{
	auto p4 = Point3DH(_position);
	auto distanceVec = p4 - p;
	auto distance = distanceVec.Length();
	float linearFallOff;
	if (distance >= _falloffEndDistance) linearFallOff = 0.f;
	else if (distance <= _falloffStartDistance) linearFallOff = 1.f;
	else linearFallOff = 1.f - (distance - _falloffStartDistance) / (_falloffEndDistance - _falloffStartDistance);
	auto cosineTheta = -(_direction.Dot(distanceVec.AsVector4f().xyz().Normalize()));
	auto a = max(cosineTheta, 0.f);
	auto delta = std::pow(a, _power)*linearFallOff;
	return delta * _irradiance;
}

Luxko::Vector3DH Luxko::Anuthur::SpotLightLinear::GetLightVectorAtWorldCoor(const Point3DH& p) const
{
	auto p4 = Point3DH(_position);
	auto distanceVec = p4 - p;
	return distanceVec.Normalize();
}

std::pair<Luxko::Vector3DH, Luxko::Anuthur::ColorRGBf> Luxko::Anuthur::SpotLightLinear::GetAttributesAtWorldCoor(const Point3DH& p) const
{
	auto p4 = Point3DH(_position);
	auto distanceVec = p4 - p;
	auto distance = distanceVec.Length();
	float linearFallOff;
	if (distance >= _falloffEndDistance) linearFallOff = 0.f;
	else if (distance <= _falloffStartDistance) linearFallOff = 1.f;
	else linearFallOff = 1.f - (distance - _falloffStartDistance) / (_falloffEndDistance - _falloffStartDistance);
	distanceVec = distanceVec.Normalize();
	auto cosineTheta = -(_direction.Dot(distanceVec.AsVector4f().xyz()));
	auto a = max(cosineTheta, 0.f);
	auto delta = std::pow(a, _power)*linearFallOff;
	return{ distanceVec , delta * _irradiance };
}
