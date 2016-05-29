//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
struct BasicLight
{
	float4	PosAndFallStart;
	float4	IrradianceAndFallEnd;
	float4	DirectionAndPower;
};

struct BlinnPhongMaterial
{
	float4	R0andRoughness;
	float3	DiffuseAlbedo;
	//float offset;
};

float Attenuation(float d, float falloffStart, float falloffEnd)
{
	return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

float3 SchlickApproximation(float3 R0, float3 normal, float3 lightDirec)
{
	float t = 1.f - dot(normal, lightDirec);
	return R0 + (1.f - R0)*(t*t*t*t*t);
}

float3 BlinnPhong(float3 irradiance, float3 light, float3 normal, float3 eye, BlinnPhongMaterial mat)
{
	const float m = mat.R0andRoughness.w * 256.f;
	float3 h = normalize(eye + light);
	float rf = ((m + 8.f) / 8.f)*pow(max(dot(h, normal), 0.f), m);
	float3 Rtheta = SchlickApproximation(mat.R0andRoughness.xyz, normal, light);
	float3 specAlbedo = Rtheta*rf;

	// critical: get HDR back to LDR
	specAlbedo = specAlbedo / (specAlbedo + 1.f);
	return (mat.DiffuseAlbedo + specAlbedo) * irradiance;
}

float3 EvaluateDirectionalLight(BasicLight L, BlinnPhongMaterial mat, float3 normal, float3 eye)
{
	float3 light = -(L.DirectionAndPower.xyz);
	float3 ls = L.IrradianceAndFallEnd.xyz /** max(dot(light, normal), 0.f)*/;
	return BlinnPhong(ls, light, normal, eye, mat);
}

float3 EvaluatePointLightLinear(BasicLight L, BlinnPhongMaterial mat, float3 pos, float3 normal, float3 eye)
{
	float3 light = L.PosAndFallStart.xyz - pos;
	float d = length(light);
	float att = Attenuation(d, L.PosAndFallStart.w, L.IrradianceAndFallEnd.w);
	float3 irradiance = att*L.IrradianceAndFallEnd.xyz;
	light = normalize(light);
	return BlinnPhong(irradiance, light, normal, eye, mat);
}

float3 EvaluatePointLightQuadra(BasicLight L, BlinnPhongMaterial mat, float3 pos, float3 normal, float3 eye)
{
	float3 light = L.PosAndFallStart.xyz - pos;
	float d = length(light);
	float falloff =  d*L.IrradianceAndFallEnd.w + 1.f;
	falloff *= falloff;
	float3 irradiance = L.IrradianceAndFallEnd.xyz / falloff;
	light /= d;
	return BlinnPhong(irradiance, light, normal, eye, mat);
}

float3 EvaluateSpotLightLinear(BasicLight L, BlinnPhongMaterial mat, float3 pos, float3 normal, float3 eye)
{
	float3 light = L.PosAndFallStart.xyz - pos;
	float d = length(light);
	light /= d;
	float att = Attenuation(d, L.PosAndFallStart.w, L.IrradianceAndFallEnd.w);
	float spotFactor = pow(max(dot(light, -L.DirectionAndPower.xyz), 0.f), L.DirectionAndPower.w) * att;
	float3 irradiance = spotFactor*L.IrradianceAndFallEnd.xyz;
	return BlinnPhong(irradiance, light, normal, eye, mat);
}