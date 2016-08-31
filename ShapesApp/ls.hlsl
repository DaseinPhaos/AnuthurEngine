#include "LightShadero.hlsl"
cbuffer MeshWorld : register(b0)
{
	float4x4 MWmatrix;
	float4x4 MWnmatrix;
}

cbuffer MeshMaterial : register(b1)
{
	BlinnPhongMaterial material;
}

cbuffer WorldCamera : register(b2)
{
	float4x4 WCmatrix;
	float4 eyePosW;
}

cbuffer WorldLights : register(b3)
{
	BasicLight dir;
	BasicLight pointLinear;
	BasicLight pointQuadra;
	BasicLight spotlight;
}

struct VSInput
{
	float4 posL : POSITION;
	float3 normL : NORMAL;
};

struct VSOutput
{
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float3 normW : NORMAL;
};

VSOutput VS(VSInput vin)
{
	VSOutput vout = (VSOutput)0.f;
	float4 posW = mul(vin.posL, MWmatrix);
	vout.posW = posW.xyz;

	vout.normW = mul(vin.normL, (float3x3)MWnmatrix);

	vout.posH = mul(posW, WCmatrix);
	return vout;
}


float4 PS(VSOutput pin) : SV_Target
{
	pin.normW = normalize(pin.normW);

	float3 eyeW = normalize(eyePosW.xyz - pin.posW);

	//float4 ambient = float4(0.1f, 0.1f, 0.1f, 1.f);
	float3 color = EvaluateDirectionalLight(dir, material, pin.normW, eyeW);
	color += EvaluatePointLightLinear(pointLinear, material, pin.posW, pin.normW, eyeW);
	//color += EvaluatePointLightQuadra(pointQuadra, material, pin.posW, pin.normW, eyeW);
	color += EvaluateSpotLightLinear(spotlight, material, pin.posW, pin.normW, eyeW);
	//color *= 25.f;
	return float4(color, 1.f);
}