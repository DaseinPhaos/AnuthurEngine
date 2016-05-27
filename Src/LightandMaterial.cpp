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

Luxko::Anuthur::LambertMaterial::LambertMaterial(const ColorRGBf& diffuseAlbedo)
{
	_diffuseAlbedo = diffuseAlbedo;
}

Luxko::Anuthur::LambertMaterial::LambertMaterial(float ar, float ag, float ab):_diffuseAlbedo(ar, ag, ab)
{}

Luxko::Anuthur::ColorRGBf Luxko::Anuthur::LambertMaterial::ApplyDirectionalLight(const ColorRGBf& irradiance, const Vector3DH& lightVec, const Vector3DH& normalVec, const Vector3DH& eyeVec) const
{
	auto cosineTheta = lightVec*normalVec;
	if (cosineTheta <= 0.f)return{ 0.f, 0.f, 0.f };
	return (cosineTheta)*(irradiance.ElementWideMultiply(_diffuseAlbedo));
}

Luxko::Anuthur::ColorRGBf Luxko::Anuthur::LambertMaterial::ApplyIndirectionalLight(const ColorRGBf& irradiance, const Vector3DH& normalVec, const Vector3DH& eyeVec) const
{
	return irradiance.ElementWideMultiply(_diffuseAlbedo);
}

Luxko::Anuthur::ColorRGBf Luxko::Anuthur::SpecularMaterial::SchlickApproximation(const ColorRGBf& R0, float cosineTheta)
{
	static const auto oneColor = ColorRGBf(1.f, 1.f, 1.f);
	return R0 + ( oneColor - R0)*(std::pow(1.f - cosineTheta, 5.f));
}

Luxko::Anuthur::SpecularMaterial::SpecularMaterial(const ColorRGBf& frenselAlbedo0, float roughness)
{
	_frenselAlbedo0 = frenselAlbedo0;
	_roughness = roughness;
}

Luxko::Anuthur::ColorRGBf Luxko::Anuthur::SpecularMaterial::ApplyDirectionalLight(const ColorRGBf& irradiance, const Vector3DH& lightVec, const Vector3DH& normalVec, const Vector3DH& eyeVec) const
{
	auto cosineTheta = lightVec*normalVec;
	if (cosineTheta <= 0.f) return{ 0.f, 0.f, 0.f };
	auto hVec = (normalVec + eyeVec).Normalize();
	auto cosineAlpha = hVec*eyeVec;
	auto frenselAlbedoAlpha = SchlickApproximation(_frenselAlbedo0, cosineAlpha);
	auto m = _roughness*256.f;
	auto roughnessCoefficient = ((m + 8.f) / 8.f)*std::pow(normalVec*hVec, m);
	return (roughnessCoefficient*cosineTheta)*(irradiance.ElementWideMultiply(frenselAlbedoAlpha));
}

Luxko::Anuthur::ColorRGBf Luxko::Anuthur::SpecularMaterial::ApplyIndirectionalLight(const ColorRGBf& irradiance, const Vector3DH& normalVec, const Vector3DH& eyeVec) const
{
	return{ 0.f, 0.f, 0.f };
}

Luxko::Anuthur::BlinnPhongMaterial::BlinnPhongMaterial(const ColorRGBf& diffuseAlbedo, const ColorRGBf& frenselAlbedo0, float roughness)
	:_spec(frenselAlbedo0, roughness), _diffuse(diffuseAlbedo)
{ }

Luxko::Anuthur::BlinnPhongMaterial::BlinnPhongMaterial(const LambertMaterial& diffuse, const SpecularMaterial& specular)
	:_spec(specular), _diffuse(diffuse)
{ }

Luxko::Anuthur::ColorRGBf Luxko::Anuthur::BlinnPhongMaterial::ApplyDirectionalLight(const ColorRGBf& irradiance, const Vector3DH& lightVec, const Vector3DH& normalVec, const Vector3DH& eyeVec) const
{
	return _spec.ApplyDirectionalLight(irradiance, lightVec, normalVec, eyeVec)
		+ _diffuse.ApplyDirectionalLight(irradiance, lightVec, normalVec, eyeVec);
}

Luxko::Anuthur::ColorRGBf Luxko::Anuthur::BlinnPhongMaterial::ApplyIndirectionalLight(const ColorRGBf& irradiance, const Vector3DH& normalVec, const Vector3DH& eyeVec) const
{
	return _diffuse.ApplyIndirectionalLight(irradiance, normalVec, eyeVec);
}
