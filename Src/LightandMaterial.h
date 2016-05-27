//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "RendererPCH.h"
#include "Vector3f.h"
#include "Vector4f.h"
#include "Point3DH.h"
#include "Vector3DH.h"

// We define interface of light and material as follows.
// Implementation classes should be aware whether they are to be
// provided to the GPU as constant buffer data or something else,
// and be aligned that way. Corresponding shader program should
// also be defined.
//
// An implementation utilizing a basic Diffuse-Specular material
// is provided. Some basic lights are also provided.

namespace Luxko {
	namespace Anuthur {
		using ColorRGBf = Vector3f;
		using ColorRGBAf = Vector4f;
		class ILight abstract {
		public:
			virtual ~ILight() = default;
			virtual ColorRGBf GetIrradianceAtWorldCoor(const Point3DH& p)const = 0;
			virtual Vector3DH GetLightVectorAtWorldCoor(const Point3DH& p)const = 0;
			virtual std::pair<Vector3DH, ColorRGBf> GetAttributesAtWorldCoor(const Point3DH& p)const = 0;
		};

		class IMaterial abstract {
		public:
			virtual ~IMaterial() = default;
			virtual ColorRGBf ApplyDirectionalLight(const ColorRGBf& irradiance, const Vector3DH& lightVec,
				const Vector3DH& normalVec, const Vector3DH& eyeVec)const = 0;
			virtual ColorRGBf ApplyIndirectionalLight(const ColorRGBf& irradiance, const Vector3DH& normalVec,
				const Vector3DH& eyeVec)const = 0;
		};

		// irradiance = _irrandiance *( 1 -  saturate(\frac{d - _falloffStart}{_falloffEnd - _falloffStart}))
		class ANUTHURRENDERER_API PointLightLinear : public ILight {
		public:
			//PointLightLinear() = default;
			PointLightLinear(const Point3DH& position, const ColorRGBf& strength,
				float falloffStart, float fallOffEnd);
			PointLightLinear(const PointLightLinear&) = default;
			PointLightLinear& operator=(const PointLightLinear&) = default;
			virtual ~PointLightLinear() = default;
			virtual ColorRGBf GetIrradianceAtWorldCoor(const Point3DH& p)const override;
			virtual Vector3DH GetLightVectorAtWorldCoor(const Point3DH& p)const override;
			virtual std::pair<Vector3DH, ColorRGBf> GetAttributesAtWorldCoor(const Point3DH& p)const override;

			Vector3f	_position;
			float		_falloffStartDistance;
			ColorRGBf	_irradiance;
			float		_falloffEndDistance;
		};

		// irradiance = _irradiance *( d * _falloffCoefficient + 1 )^2
		class ANUTHURRENDERER_API PointLightQuadra : public ILight {
		public:
			//PointLightQuadra() = default;
			PointLightQuadra(const Point3DH& position, const ColorRGBf& strength,
				float falloffCoefficient);
			PointLightQuadra(const PointLightQuadra&) = default;
			PointLightQuadra& operator=(const PointLightQuadra&) = default;
			virtual ~PointLightQuadra() = default;
			virtual ColorRGBf GetIrradianceAtWorldCoor(const Point3DH& p)const override;
			virtual Vector3DH GetLightVectorAtWorldCoor(const Point3DH& p)const override;
			virtual std::pair<Vector3DH, ColorRGBf> GetAttributesAtWorldCoor(const Point3DH& p)const override;

			Point3DH	_position;
			ColorRGBf	_irradiance;
			float		_falloffCoefficient;

		};

		// irradiance = _irradiance
		class ANUTHURRENDERER_API DirectionalLight : public ILight {
		public:
			//DirectionalLight() = default;
			DirectionalLight(const Vector3DH& direction, const ColorRGBf& strength);
			DirectionalLight(const DirectionalLight&) = default;
			DirectionalLight& operator=(const DirectionalLight&) = default;
			virtual ~DirectionalLight() = default;
			virtual ColorRGBf GetIrradianceAtWorldCoor(const Point3DH& p)const override;
			virtual Vector3DH GetLightVectorAtWorldCoor(const Point3DH& p)const override;
			virtual std::pair<Vector3DH, ColorRGBf> GetAttributesAtWorldCoor(const Point3DH& p)const override;


			Vector3DH	_direction;
			ColorRGBf	_irradiance;
		};

		// irradiance = _irrandiance*max(cos(\theta), 0)^_power*LinearFallOff
		// \theta = _direction * (p - _pos).normalize
		// LinearFallOff = 1 - saturate((d - _falloffStart)/(_falloffEnd - _falloffEnd))
		class ANUTHURRENDERER_API SpotLightLinear : public ILight {
		public:
			//SpotLightLinear() = default;
			SpotLightLinear(const Point3DH& position, const ColorRGBf& strength,
				float falloffStart, float falloffEnd, const Vector3DH& direction,
				float power = 8.f);
			SpotLightLinear(const SpotLightLinear&) = default;
			SpotLightLinear& operator=(const SpotLightLinear&) = default;
			virtual ~SpotLightLinear() = default;
			virtual ColorRGBf GetIrradianceAtWorldCoor(const Point3DH& p)const override;
			virtual Vector3DH GetLightVectorAtWorldCoor(const Point3DH& p)const override;
			virtual std::pair<Vector3DH, ColorRGBf> GetAttributesAtWorldCoor(const Point3DH& p)const override;


			Vector3f	_position;
			float		_falloffStartDistance;
			ColorRGBf	_irradiance;
			float		_falloffEndDistance;
			Vector3f	_direction;
			float		_power;

		};

		struct LambertMaterialData {
			ColorRGBf DiffuseAlbedo;
		};
		// Lambert surface
		class ANUTHURRENDERER_API LambertMaterial: public IMaterial {
		public:
			LambertMaterial() = default;
			explicit LambertMaterial(const ColorRGBf& diffuseAlbedo);
			LambertMaterial(float ar, float ag, float ab);
			LambertMaterial(const LambertMaterial&) = default;
			LambertMaterial& operator=(const LambertMaterial&) = default;
			virtual ~LambertMaterial() = default;
			virtual ColorRGBf ApplyDirectionalLight(const ColorRGBf& irradiance, const Vector3DH& lightVec,
				const Vector3DH& normalVec, const Vector3DH& eyeVec)const override;
			virtual ColorRGBf ApplyIndirectionalLight(const ColorRGBf& irradiance, const Vector3DH& normalVec,
				const Vector3DH& eyeVec)const override;
			LambertMaterialData GetData()const { return{ _diffuseAlbedo }; }
			ColorRGBf	_diffuseAlbedo;
		};
		
		struct SpecularMaterialData {
			ColorRGBf	FrenselAlbedo0;
			float		Roughness;
		};
		// Specular surface
		class ANUTHURRENDERER_API SpecularMaterial : public IMaterial {
		public:
			SpecularMaterial() = default;
			static ColorRGBf SchlickApproximation(const ColorRGBf& R0, float cosineTheta);
			SpecularMaterial(const ColorRGBf& frenselAlbedo0, float roughness);
			SpecularMaterial(const SpecularMaterial&) = default;
			SpecularMaterial& operator=(const SpecularMaterial&) = default;
			virtual ~SpecularMaterial() = default;
			virtual ColorRGBf ApplyDirectionalLight(const ColorRGBf& irradiance, const Vector3DH& lightVec,
				const Vector3DH& normalVec, const Vector3DH& eyeVec)const override;
			virtual ColorRGBf ApplyIndirectionalLight(const ColorRGBf& irradiance, const Vector3DH& normalVec,
				const Vector3DH& eyeVec)const override;
			SpecularMaterialData GetData()const { return{ _frenselAlbedo0, _roughness }; }
			ColorRGBf	_frenselAlbedo0;
			float		_roughness;
		};

		struct BlinnPhongMaterialData {
			SpecularMaterialData SpecularData;
			LambertMaterialData DiffuseData;
		};
		// BlinnPhong material
		class ANUTHURRENDERER_API BlinnPhongMaterial : public IMaterial {
		public:
			BlinnPhongMaterial() = default;
			BlinnPhongMaterial(const ColorRGBf& diffuseAlbedo, const ColorRGBf& frenselAlbedo0, float roughness);
			BlinnPhongMaterial(const LambertMaterial& diffuse, const SpecularMaterial& specular);
			BlinnPhongMaterial(const BlinnPhongMaterial&) = default;
			BlinnPhongMaterial& operator=(const BlinnPhongMaterial&) = default;
			virtual ~BlinnPhongMaterial() = default;
			virtual ColorRGBf ApplyDirectionalLight(const ColorRGBf& irradiance, const Vector3DH& lightVec,
				const Vector3DH& normalVec, const Vector3DH& eyeVec)const override;
			virtual ColorRGBf ApplyIndirectionalLight(const ColorRGBf& irradiance, const Vector3DH& normalVec,
				const Vector3DH& eyeVec)const override;
			BlinnPhongMaterialData GetData()const {
				return{ _spec.GetData(), _diffuse.GetData() };
			}

			SpecularMaterial	_spec;
			LambertMaterial		_diffuse;

		};
	}
}
