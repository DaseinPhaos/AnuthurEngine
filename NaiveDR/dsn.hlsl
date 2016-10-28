//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

cbuffer Transforms: register(b0)
{
	float4x4 mWorld;
	float4x4 mWorldView;
	float4x4 mWorldViewProj;
};

cbuffer Materials: register(b1)
{
	float3 SpecularAlbedo;
	float SpecularPower;
};

Texture2D DiffuseMap: register(t0);
Texture2D NormalMap: register(t1);
SamplerState Sampler: register(s0);

struct VSI
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float3 tangent: TANGENT;
	float2 tex: TEXCOORD;
};

struct VSO
{
	float4 pos: SV_POSITION;
	float3 posW: POSITION_W;
	float3 normW: NORM_W;
	float3 tangentW: TANGENT_W;
	float2 tex: TEXCOORD;
};

struct PSO
{
	float4 norm: SV_Target0;
	float4 da: SV_Target1;
	float4 sa: SV_Target2;
	float4 pos: SV_Target3;
};

VSO VS(in VSI vsi) {
	VSO vso;
	vso.posW = mul(vsi.pos, mWorld).xyz;
	// vso.pos = mul(vsi.pos, mWorldView);
	vso.pos = mul(vsi.pos, mWorldViewProj);
	vso.normW = normalize(mul(vsi.norm, (float3x3)mWorld));
	vso.tangentW = normalize(mul(vsi.tangent, (float3x3)mWorld));
	vso.tex = vsi.tex;

	return vso;
}

PSO PS(in VSO psi) {
	float3 normalT = NormalMap.Sample(Sampler, psi.tex).xyz;
	// normalT = normalize(normalT*2.f - 1.f);
	normalT = normalize(normalT);
	float3 norm = normalize(psi.normW);
	float3 tangent = normalize(psi.tangentW);
	float3 bitangent = normalize(cross(norm, tangent));
	float3x3 tFrameW = float3x3 (tangent, bitangent, norm);

	float3 normW = mul(normalT, tFrameW);

	PSO pso;
	pso.norm = float4(normW, 1.f);

	float3 da = DiffuseMap.Sample(Sampler, psi.tex).xyz;
	pso.da = float4(da, 1.f);
	pso.sa = float4(SpecularAlbedo, SpecularPower);
	pso.pos = float4(psi.posW, 1.f);
	return pso;
}