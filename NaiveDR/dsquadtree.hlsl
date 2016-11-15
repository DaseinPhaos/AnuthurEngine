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
	float qdmMaxMipLvl;
};

cbuffer MaterialCB: register(b1)
{
	MaterialAttr material;
};

Texture2D DiffuseMap: register(t0);
Texture2D NormalMap: register(t1);
Texture2D QuadDispMap: register(t2);

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
	float3 posV: POSITION_V;
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
}

// `viewRayT` should be a normalized vector
// pointing from the viewport to the interception point.
float3 QDMSearch(int MaxMipLevel, float3 viewRayT, float3 posT) {
	float currHeight;
	float3 lastPos = posT;
	int currLevel = MaxMipLevel;

	int2 dircSign = sign(viewRayT.xy);

	while (currLevel >= 0) {
		uint tWidth;
		uint tHeight;
		uint numOfLvls;
		QuadDispMap.GetDimensions(uint(currLevel), tWidth, wHeight, numOfLvls);
		int2 lastPosID = int2(lastPos.xy*float2(tWidth, tHeight));
		currHeight = QuadDispMap.Load(int3(lastPosID, currLevel));
		if (currHeight < lastPos.z) {
			float3 currPos = posT + viewRayT * (1.f - currHeight);
			int2 currPosID = int2(currPos*float2(tWidth, tHeight));
			if (currPosID != lastPosID) {
				float2 dlp = lastPos.xy - posT.xy;
				float2 rayingPos = (lastPosID + dircSign) / float2(tWidth, tHeight);
				float2 drp = rayingPos - posT.xy;
				float2 dnc = abs((1.f - lastPos.z) * drp / dlp);
				currHeight = max(currHeight, dnc.x, dnc.y);
				currLevel += 2;
				currPos = posT + viewRayT * (1.f - currHeight);
			}
			lastPos = currPos;
		}
		currLevel--;
	}
	return lastPos;
}

VSO VSMain(in VSI vsi) {
	VSO vso;
	float4 posW = mul(vsi.pos, material.mOtoW);
	vso.pos = mul(posW, camera.mWtoH);
	vso.posV = mul(posW, camera.mWtoV);
	vso.normW = normalize(mul(vsi.norm, (float3x3)material.mOtoW));
	vso.tangentW = normalize(mul(vsi.tangent, (float3x3)material.mOtoW));
	vso.tex = vsi.tex;

	return vso;
}



PSO PSMain(in VSO psi) {
	float3 norm = normalize(psi.normW);
	float3 tangent = normalize(psi.tangentW);
	float3 bitangent = normalize(cross(norm, tangent));
	float3x3 tFrameW = float3x3(tangent, bitangent, norm);
	float3 posW = mul(psi.posV, float3x3(camera.mVtoW));
	float3 viewRayW = normalize(posW - camera.posW);
	float3 adjustedPosT = QDMSearch(
		material.qdmMaxMipLvl,
		mul(viewRayW, tFrameW),
		float3(psi.tex, 1.f));
	float3 normalT = NormalMap.Sample(Sampler, adjustedPosT.xy).xyz;
	normalT = normalT * 2.f - 1.f;
	float3 normW = mul(normalT, tFrameW);

	PSO pso;
	pso.npp = float4(SpheremapEncode(normW), 
		psi.posV.z / camera.farClipD, material.sPower);
	pso.da = DiffuseMap.Sample(Sampler, psi.tex);
	pso.sa = float4(material.sAlbedo, 1.f);
	return pso;
}