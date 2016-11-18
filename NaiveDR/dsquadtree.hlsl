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
	int currLevel = MaxMipLevel-1;
	int2 dircSign = sign(viewRayT.xy);

	uint count = 0;
	while (currLevel >= 0) {
		uint tWidth = exp2(MaxMipLevel - currLevel - 1);
		uint tHeight = tWidth;
		float stepCo = 1.f;
		//uint tWidth;
		//uint tHeight;
		//uint numOfLvls;
		//QuadDispMap.GetDimensions(uint(currLevel), tWidth, tHeight, numOfLvls);

		int2 lastPosCellID = int2(lastPos.xy*float2(tWidth, tHeight));
		currHeight = QuadDispMap.Load(int3(lastPosCellID, currLevel));

		if (currHeight < lastPos.z) {
			float3 currPos = startPos + viewRayT * stepCo * (1.f - currHeight);
			int2 currPosCellID = int2(currPos.xy * float2(tWidth, tHeight));
			if (currPosCellID.x != lastPosCellID.x || currPosCellID.y != lastPosCellID.y) {
				float2 dlp = lastPos.xy - startPos.xy;
				float2 rayingPos = (lastPosCellID + dircSign) / float2(tWidth, tHeight);
				float2 drp = rayingPos - startPos.xy;
				float2 dnc = abs((1.f - lastPos.z) * drp / dlp);
				//currHeight = max(currHeight, max((1.f - dnc.x), (1.f - dnc.y)));
				float currDepth = min(1.f - currHeight, min(dnc.x, dnc.y));
				currHeight = 1.f - currDepth;
				currLevel += 2;
				//if (currLevel > MaxMipLevel - 1) currLevel = MaxMipLevel - 1;
				currPos = startPos + viewRayT * stepCo * (1.f - currHeight);
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
	//VSO vso;
	//float4 posW = mul(vsi.pos, material.mOtoW);
	//vso.pos = mul(posW, camera.mWtoH);
	//vso.posV = mul(posW, camera.mWtoV).xyz;
	//vso.normW = normalize(mul(vsi.norm, (float3x3)material.mOtoW));
	//vso.tangentW = normalize(mul(vsi.tangent, (float3x3)material.mOtoW));
	//vso.tex = vsi.tex;

	//return vso;

	VSO vso;
	float4 posW = mul(vsi.pos, material.mOtoW);
	vso.pos = mul(posW, camera.mWtoH);
	vso.posW = posW.xyz;
	vso.normW = normalize(mul(float4(vsi.norm, 0.f), material.mOtoW).xyz);
	vso.tangentW = normalize(mul(float4(vsi.tangent, 0.f), material.mOtoW).xyz);
	vso.tex = vsi.tex;

	return vso;
}



PSO PSMain(in VSO psi) {
	//float3 norm = normalize(psi.normW);
	////float3 tangent = normalize(psi.tangentW);
	//float3 tangent = normalize(psi.tangentW - dot(psi.tangentW, norm) * norm);
	//float3 bitangent = normalize(cross(norm, tangent));
	//float3x3 tFrameW = float3x3(tangent, bitangent, norm);
	//float3 posW = mul(float4(psi.posV, 1.f), camera.mVtoW).xyz;
	//float3 viewRayW = posW - camera.posW;
	//float3 adjustedPosT = QDMSearch(
	//	material.qdmMaxMipLvl,
	//	normalize(mul(tFrameW, viewRayW)),
	//	float3(psi.tex, 1.f));
	////float3 normalT = NormalMap.Sample(Sampler, adjustedPosT.xy).xyz;
	//float3 normalT = NormalMap.Sample(PointSampler, adjustedPosT.xy).xyz;
	//normalT = normalT * 2.f - 1.f;
	//float3 normW = mul(normalT, tFrameW);
	//PSO pso;
	//pso.npp = float4(SpheremapEncode(normW), 
	//	psi.posV.z / camera.farClipD, material.sPower);
	////pso.da = DiffuseMap.Sample(Sampler, adjustedPosT.xy);
	//pso.da = float4(1.f, 1.f, 1.f, 1.f);
	//pso.sa = float4(material.sAlbedo, 1.f);
	//return pso;

	//float3 normalT = NormalMap.Sample(Sampler, psi.tex).xyz;
	//normalT = normalT * 2.f - 1.f;
	//float3 norm = normalize(psi.normW);
	//float3 tangent = normalize(psi.tangentW);
	////float3 tangent = normalize(psi.tangentW - dot(psi.tangentW, norm) * norm);
	//float3 bitangent = normalize(cross(norm, tangent));
	//float3x3 tFrameW = float3x3 (tangent, bitangent, norm);

	//float3 normW = mul(normalT, tFrameW);

	//PSO pso;
	////pso.npp = float4(1.f, 0.5f, 0.3f, 1.f);
	//pso.npp = float4(SpheremapEncode(normW), psi.posV.z / camera.farClipD, material.sPower);

	////pso.da = DiffuseMap.Sample(Sampler, psi.tex);
	//pso.da = float4(1.f, 1.f, 1.f, 1.f);
	//pso.sa = float4(material.sAlbedo, 1.f);
	//return pso;

	float3 norm = normalize(psi.normW);
	//float3 tangent = normalize(psi.tangentW);
	float3 tangent = normalize(psi.tangentW - dot(psi.tangentW, norm) * norm);
	float3 bitangent = normalize(cross(norm, tangent));
	float3x3 tFrameW = float3x3(tangent, bitangent, norm);
	float3 posW = psi.posW;
	float3 viewRayW = normalize(posW - camera.posW);
	float3 viewRayT = normalize(mul(tFrameW, viewRayW));
	float3 startPos = float3(psi.tex, 0.f) - viewRayT * QuadDispMap.Load(int3(psi.tex*exp2(material.qdmMaxMipLvl - 1), material.qdmMaxMipLvl - 1));
	float3 adjustedPosT = QDMSearch(
		material.qdmMaxMipLvl,
		viewRayT,
		float3(psi.tex, 0.f));
	//float3 normalT = NormalMap.Load(int3(int2(adjustedPosT.xy * exp2(material.qdmMaxMipLvl - 1)), 0)).xyz;
	float3 normalT = NormalMap.Sample(LinearSampler, adjustedPosT.xy).xyz;
	normalT = normalT * 2.f - 1.f;
	float3 normW = mul(normalT, tFrameW);
	PSO pso;
	pso.npp = float4(SpheremapEncode(normW),
		mul(float4(posW, 1.f), camera.mWtoV).z / camera.farClipD, material.sPower);
	pso.da = DiffuseMap.Sample(Sampler, adjustedPosT.xy);
	//pso.da = float4(1.f, 1.f, 1.f, 1.f);
	pso.sa = float4(material.sAlbedo, 1.f);
	return pso;

	//float3 norm = normalize(psi.normW);
	//PSO pso;
	//pso.npp = float4(SpheremapEncode(norm),
	//	mul(float4(psi.posW, 1.f), camera.mWtoV).z / camera.farClipD, material.sPower);
	//float fuck = QuadDispMap.Load(int3(uint2(psi.tex*16.f), 5));
	//pso.da = float4(0.f, 0.f, fuck, 1.f);
	//pso.sa = float4(material.sAlbedo, 1.f);
	//return pso;

}

////**********************************************************************
//// This file is part of the Anuthur Engine. It is distributed under
//// the MIT license(https://opensource.org/licenses/MIT).
////
//// Copyright (c) Dasein Phaos aka. Luxko.
////**********************************************************************
//
//struct CameraAttr {
//	float4x4 mWtoV;
//	float4x4 mVtoW;
//	float4x4 mWtoH;
//	float3 posW;
//	float farClipD;
//};
//
//cbuffer CameraCB: register(b0)
//{
//	CameraAttr camera;
//};
//
//struct MaterialAttr {
//	float4x4 mOtoW;
//	float3 sAlbedo;
//	float sPower;
//	uint qdmMaxMipLvl;
//};
//
//cbuffer MaterialCB: register(b1)
//{
//	MaterialAttr material;
//};
//
//Texture2D DiffuseMap: register(t0);
//Texture2D NormalMap: register(t1);
//Texture2D<float> QuadDispMap: register(t2);
//
//SamplerState Sampler: register(s0);
//SamplerState PointSampler: register(s1);
//
//struct VSI
//{
//	float4 pos: POSITION;
//	float3 norm: NORMAL;
//	float3 tangent: TANGENT;
//	float2 tex: TEXCOORD;
//};
//
//struct VSO
//{
//	float4 pos: SV_POSITION;
//	float3 posV: POSITION_V;
//	float3 viewT: VIEW_T;
//	float3 normW: NORM_W;
//	float3 tangentW: TANGENT_W;
//	float2 tex: TEXCOORD;
//};
//
//struct PSO
//{
//	float4 npp: SV_Target0;
//	float4 da: SV_Target1;
//	float4 sa: SV_Target2;
//};
//
//// Sphere map transform, Mittring 2009
//float2 SpheremapEncode(float3 norm) {
//	return normalize(norm.xy) * (sqrt(-norm.z * 0.5f + 0.5f));
//}
//
//
//
//// `viewRayT` should be a normalized vector
//// pointing from the viewport to the interception point.
//float3 QDMSearch(int MaxMipLevel, float3 viewRayT, float3 posT) {
//	float currHeight;
//	float3 lastPos = posT;
//	int currLevel = MaxMipLevel - 1;
//	int2 dircSign = sign(viewRayT.xy);
//
//	uint count = 0;
//	while (currLevel >= 0) {
//		//uint tWidth = exp2(MaxMipLevel - currLevel - 1);
//		//uint tHeight = tWidth;
//
//		uint tWidth;
//		uint tHeight;
//		uint numOfLvls;
//		QuadDispMap.GetDimensions(uint(currLevel), tWidth, tHeight, numOfLvls);
//		int2 lastPosCellID = int2(lastPos.xy*float2(tWidth, tHeight));
//		currHeight = QuadDispMap.Load(int3(lastPosCellID, currLevel));
//		if (currHeight < lastPos.z) {
//			float3 currPos = posT + viewRayT * (1.f - currHeight);
//			int2 currPosCellID = int2(currPos.xy * float2(tWidth, tHeight));
//			if (currPosCellID.x != lastPosCellID.x || currPosCellID.y != lastPosCellID.y) {
//				float2 dlp = lastPos.xy - posT.xy;
//				float2 rayingPos = (lastPosCellID + dircSign) / float2(tWidth, tHeight);
//				float2 drp = rayingPos - posT.xy;
//				float2 dnc = abs((1.f - lastPos.z) * drp / dlp);
//				//currHeight = max(currHeight, max((1.f - dnc.x), (1.f - dnc.y)));
//				float currDepth = min(1.f - currHeight, min(dnc.x, dnc.y));
//				currHeight = 1.f - currDepth;
//				currLevel += 2;
//				currPos = posT + viewRayT * (1.f - currHeight);
//				//return lastPos;
//			}
//			lastPos = currPos;
//		}
//		currLevel--;
//		count++;
//		if (count > 10) break;
//	}
//	return lastPos;
//}
//
//VSO VSMain(in VSI vsi) {
//	VSO vso;
//	float4 posW = mul(vsi.pos, material.mOtoW);
//	vso.pos = mul(posW, camera.mWtoH);
//	vso.posV = mul(posW, camera.mWtoV).xyz;
//	vso.normW = normalize(mul(vsi.norm, (float3x3)material.mOtoW));
//	vso.tangentW = normalize(mul(vsi.tangent, (float3x3)material.mOtoW));
//	float3 bitangent = normalize(cross(vso.normW, vso.tangentW));
//	float3x3 tFrameW = float3x3(vso.tangentW, bitangent, vso.normW);
//	vso.viewT = normalize(mul(tFrameW, posW.xyz - camera.posW));
//	vso.tex = vsi.tex;
//
//	return vso;
//
//	//VSO vso;
//	//float4 posW = mul(vsi.pos, material.mOtoW);
//	//vso.pos = mul(posW, camera.mWtoH);
//	//vso.posW = posW.xyz;
//	//vso.normW = normalize(mul(float4(vsi.norm, 0.f), material.mOtoW).xyz);
//	//vso.tangentW = normalize(mul(float4(vsi.tangent, 0.f), material.mOtoW).xyz);
//	//vso.tex = vsi.tex;
//
//	//return vso;
//}
//
//
//
//PSO PSMain(in VSO psi) {
//	//float3 norm = normalize(psi.normW);
//	////float3 tangent = normalize(psi.tangentW);
//	//float3 tangent = normalize(psi.tangentW - dot(psi.tangentW, norm) * norm);
//	//float3 bitangent = normalize(cross(norm, tangent));
//	//float3x3 tFrameW = float3x3(tangent, bitangent, norm);
//	//float3 posW = mul(float4(psi.posV, 1.f), camera.mVtoW).xyz;
//	//float3 viewRayW = posW - camera.posW;
//	//float3 adjustedPosT = QDMSearch(
//	//	material.qdmMaxMipLvl,
//	//	normalize(mul(tFrameW, viewRayW)),
//	//	float3(psi.tex, 1.f));
//	////float3 normalT = NormalMap.Sample(Sampler, adjustedPosT.xy).xyz;
//	//float3 normalT = NormalMap.Sample(PointSampler, adjustedPosT.xy).xyz;
//	//normalT = normalT * 2.f - 1.f;
//	//float3 normW = mul(normalT, tFrameW);
//	//PSO pso;
//	//pso.npp = float4(SpheremapEncode(normW), 
//	//	psi.posV.z / camera.farClipD, material.sPower);
//	////pso.da = DiffuseMap.Sample(Sampler, adjustedPosT.xy);
//	//pso.da = float4(1.f, 1.f, 1.f, 1.f);
//	//pso.sa = float4(material.sAlbedo, 1.f);
//	//return pso;
//
//	//float3 normalT = NormalMap.Sample(Sampler, psi.tex).xyz;
//	//normalT = normalT * 2.f - 1.f;
//	//float3 norm = normalize(psi.normW);
//	//float3 tangent = normalize(psi.tangentW);
//	////float3 tangent = normalize(psi.tangentW - dot(psi.tangentW, norm) * norm);
//	//float3 bitangent = normalize(cross(norm, tangent));
//	//float3x3 tFrameW = float3x3 (tangent, bitangent, norm);
//
//	//float3 normW = mul(normalT, tFrameW);
//
//	//PSO pso;
//	////pso.npp = float4(1.f, 0.5f, 0.3f, 1.f);
//	//pso.npp = float4(SpheremapEncode(normW), psi.posV.z / camera.farClipD, material.sPower);
//
//	////pso.da = DiffuseMap.Sample(Sampler, psi.tex);
//	//pso.da = float4(1.f, 1.f, 1.f, 1.f);
//	//pso.sa = float4(material.sAlbedo, 1.f);
//	//return pso;
//
//	float3 norm = normalize(psi.normW);
//	//float3 tangent = normalize(psi.tangentW);
//	float3 tangent = normalize(psi.tangentW - dot(psi.tangentW, norm) * norm);
//	float3 bitangent = normalize(cross(norm, tangent));
//	float3x3 tFrameW = float3x3(tangent, bitangent, norm);
//	float3 viewRayT = normalize(psi.viewT);
//	float3 adjustedPosT = QDMSearch(
//		material.qdmMaxMipLvl,
//		viewRayT,
//		float3(psi.tex, 1.f));
//	float3 normalT = NormalMap.Sample(Sampler, adjustedPosT.xy).xyz;
//	//float3 normalT = NormalMap.Sample(PointSampler, adjustedPosT.xy).xyz;
//	normalT = normalT * 2.f - 1.f;
//	float3 normW = mul(normalT, tFrameW);
//	PSO pso;
//	pso.npp = float4(SpheremapEncode(normW),
//		psi.posV.z / camera.farClipD, material.sPower);
//	//pso.da = DiffuseMap.Sample(Sampler, adjustedPosT.xy);
//	pso.da = float4(1.f, 1.f, 1.f, 1.f);
//	pso.sa = float4(material.sAlbedo, 1.f);
//	return pso;
//
//	//float3 norm = normalize(psi.normW);
//	//PSO pso;
//	//pso.npp = float4(SpheremapEncode(norm),
//	//	mul(float4(psi.posW, 1.f), camera.mWtoV).z / camera.farClipD, material.sPower);
//	//float fuck = QuadDispMap.Load(int3(uint2(psi.tex*16.f), 5));
//	//pso.da = float4(0.f, 0.f, fuck, 1.f);
//	//pso.sa = float4(material.sAlbedo, 1.f);
//	//return pso;
//
//}