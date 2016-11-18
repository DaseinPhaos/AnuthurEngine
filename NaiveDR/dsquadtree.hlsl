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
	uint qdmMaxMipLvl;
};

cbuffer MaterialCB: register(b1)
{
	MaterialAttr material;
};

Texture2D DiffuseMap: register(t0);
Texture2D NormalMap: register(t1);
Texture2D<float> QuadDispMap: register(t2);

SamplerState Sampler: register(s0);
SamplerState LinearSampler: register(s1);

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
	//float3 startPos = posT - viewRayT * QuadDispMap.Load(int3((posT*exp2(MaxMipLevel)).xy, MaxMipLevel));
	float3 startPos = posT;
	float currHeight;
	float3 lastPos = startPos;
	int currLevel = MaxMipLevel;
	int2 dircSign = sign(viewRayT.xy);

	uint count = 0;
	while (currLevel >= 0) {
		uint tWidth = exp2(MaxMipLevel - currLevel);
		uint tHeight = tWidth;


		int2 lastPosCellID = int2(lastPos.xy*float2(tWidth, tHeight));
		currHeight = QuadDispMap.Load(int3(lastPosCellID, currLevel));

		if (currHeight < lastPos.z) {
			float3 currPos = startPos + viewRayT * (1.f - currHeight);
			int2 currPosCellID = int2(currPos.xy * float2(tWidth, tHeight));
			if (currPosCellID.x != lastPosCellID.x || currPosCellID.y != lastPosCellID.y) {
				float2 dlp = lastPos.xy - startPos.xy;
				float2 rayingPos = (lastPosCellID + dircSign) / float2(tWidth, tHeight);
				float2 drp = rayingPos - startPos.xy;
				float2 dnc = abs((1.f - lastPos.z) * drp / dlp);
				float currDepth = min(1.f - currHeight, min(dnc.x, dnc.y));
				currHeight = 1.f - currDepth;
				currLevel += 2;
				currPos = startPos + viewRayT * (1.f - currHeight);
				//return lastPos;
			}
			lastPos = currPos;
		}
		currLevel--;
		count++;
		//if (count > 2 * MaxMipLevel) break;
	}
	//float finalH = NormalMap.Sample(Sampler, lastPos.xy).w;
	//lastPos = posT + viewRayT * stepCo * (1.f - finalH);
	return lastPos;
}

VSO VSMain(in VSI vsi) {
	VSO vso;
	float4 posW = mul(vsi.pos, material.mOtoW);
	vso.pos = mul(posW, camera.mWtoH);
	vso.posW = posW.xyz;
	vso.normW = normalize(mul(vsi.norm, (float3x3)material.mOtoW));
	vso.tangentW = normalize(mul(vsi.tangent, (float3x3)material.mOtoW));
	vso.tex = vsi.tex;
	return vso;
}



PSO PSMain(in VSO psi) {
	float3 norm = normalize(psi.normW);
	float3 tangent = normalize(psi.tangentW);
	//float3 tangent = normalize(psi.tangentW - dot(psi.tangentW, norm) * norm);
	float3 bitangent = normalize(cross(norm, tangent));
	float3x3 tFrameW = float3x3(tangent, bitangent, norm);
	
	float3 viewRayW = normalize(psi.posW - camera.posW);
	float3 viewRayT = normalize(mul(tFrameW, viewRayW));
	float3 startPos = float3(psi.tex, 0.f) - viewRayT * QuadDispMap.Load(int3(psi.tex*exp2(material.qdmMaxMipLvl), material.qdmMaxMipLvl));
	float3 adjustedPosT = QDMSearch(material.qdmMaxMipLvl, viewRayT, startPos);

	float3 normalT = NormalMap.Sample(LinearSampler, adjustedPosT.xy).xyz;
	normalT = normalT * 2.f - 1.f;
	float3 normW = mul(normalT, tFrameW);
	
	PSO pso;
	pso.npp = float4(SpheremapEncode(normW),
		mul(float4(psi.posW, 1.f), camera.mWtoV).z / camera.farClipD, material.sPower);
	pso.da = DiffuseMap.Sample(Sampler, adjustedPosT.xy);
	pso.sa = float4(material.sAlbedo, 1.f);
	return pso;
}