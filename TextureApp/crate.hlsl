#include "LightandMaterial.hlsl"

struct CameraData
{
	float4x4 WC;
	float4x4 WH;
	float4 Pos;
};

cbuffer CameraCB : register(b0)
{
	CameraData camera;
}

cbuffer LightsCB : register(b1)
{
	BasicLight Directional;
	BasicLight PointLinear;
	BasicLight PointQuadra;
	BasicLight Spotlight;
	float4 AmbientLight;
}

cbuffer MeshTransCB : register(b2)
{
	float4x4 CrateLW;
	float4x4 NormalLW;
}

cbuffer MaterialCB : register(b3)
{
	BlinnPhongMaterial Mat;
}

Texture2D gDiffuseMap : register(t0);

SamplerState Anisotropic : register(s0);
SamplerState Point : register(s1);
SamplerState Linear : register(s2);

struct VSInput
{
	float4 PosL : POSITION;
	float3 NormL : NORMAL;
	float2 TexL : TEXTURE;
};

struct VSOutput
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormW : NORMAL;
	float2 TexL : TEXTURE;
};

VSOutput VS(VSInput vin)
{
	float4 PosW = mul(vin.PosL, CrateLW);
	VSOutput vout;
	vout.PosH = mul(PosW, camera.WH);
	vout.PosW = PosW.xyz;
	vout.NormW = mul(vin.NormL, (float3x3)NormalLW);
	vout.TexL = vin.TexL;

	return vout;
}

float4 PS(VSOutput pin) : SV_TARGET
{
	BlinnPhongMaterial bpm;
	bpm.R0andRoughness = Mat.R0andRoughness;
	bpm.DiffuseAlbedo = gDiffuseMap.Sample(Anisotropic, pin.TexL)*Mat.DiffuseAlbedo;
	float3 eyeW = camera.Pos.xyz - pin.PosW;
	eyeW = normalize(eyeW);
	float3 color = AmbientLight.xyz * bpm.DiffuseAlbedo;
	color +=			EvaluateDirectionalLight(Directional, bpm, pin.NormW, eyeW);
	color +=			EvaluatePointLightLinear(PointLinear, bpm, pin.PosW, pin.NormW, eyeW);
	color +=			EvaluatePointLightQuadra(PointQuadra, bpm, pin.PosW, pin.NormW, eyeW);
	color +=			EvaluateSpotLightLinear(Spotlight, bpm, pin.PosW, pin.NormW, eyeW);

	return float4(color, 1.f);
}