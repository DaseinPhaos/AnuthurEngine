//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

struct CameraAttr {
	float4x4 mWtoV;
	float4x4 mVtoW;
	float4x4 mWtoH;
	float3 posW;
	float farClipD;
};

cbuffer CameraCB: register(b0)
{
	CameraAttr camera;
};

struct MaterialAttr {
	float4x4 mOtoW;
	float3 sAlbedo;
	float sPower;
};

cbuffer MaterialCB: register(b1)
{
	MaterialAttr material;
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
	// float3 posW: POSITION_W;
	float3 normW: NORM_W;
	float3 tangentW: TANGENT_W;
	float2 tex: TEXCOORD;
};

struct PSO
{
	float4 npp: SV_Target0;
	float4 da: SV_Target1;
	float4 sa: SV_Target2;
};

// Sphere map transform, Mittring 2009
float2 SpheremapEncode(float3 norm) {
	return normalize(norm.xy) * (sqrt(-norm.z * 0.5f + 0.5f));
	//return float2(atan2(norm.y, norm.x), norm.z);
}


VSO VSMain(in VSI vsi) {
	VSO vso;
	float4 posW = mul(vsi.pos, material.mOtoW);
	vso.pos = mul(posW, camera.mWtoH);
	vso.normW = normalize(mul(vsi.norm, (float3x3)material.mOtoW));
	vso.tangentW = normalize(mul(vsi.tangent, (float3x3)material.mOtoW));
	vso.tex = vsi.tex;

	return vso;
}



PSO PSMain(in VSO psi) {
	float3 normalT = NormalMap.Sample(Sampler, psi.tex).xyz;
	float3 norm = normalize(psi.normW);
	float3 tangent = normalize(psi.tangentW);
	float3 bitangent = normalize(cross(norm, tangent));
	float3x3 tFrameW = float3x3 (tangent, bitangent, norm);

	float3 normW = mul(normalT, tFrameW);

	PSO pso;
	//pso.npp = float4(1.f, 0.5f, 0.3f, 1.f);
	pso.npp = float4(SpheremapEncode(normW), psi.pos.z / camera.farClipD, material.sPower);

	pso.da = DiffuseMap.Sample(Sampler, psi.tex);
	pso.sa = float4(material.sAlbedo, 1.f);
	return pso;
}