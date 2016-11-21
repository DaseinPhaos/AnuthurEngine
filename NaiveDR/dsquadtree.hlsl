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
	float heightScale;
	float heightOffset;
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

float qdmHeightAt(float2 tUV, int2 mc, float maxH) {
	float height = QuadDispMap.Load(int3(tUV*exp2(mc.x - mc.y), mc.y));
	return height * maxH;
	//return height * vz;
}

float qdmDAt(float2 tUV, int mipD, int mipL, float scaling) {
	float height = QuadDispMap.Load(int3(tUV*mipD, mipL));
	return (1.f - height) * scaling;
	//return height * vz;
}

// `viewRayT` should be a normalized vector
// pointing from the viewport to the interception point.
//float3 QDMSearch(int MaxMipLevel, float3 viewRayT, float3 posT) {
//	////float3 startPos = posT - viewRayT * QuadDispMap.Load(int3((posT*exp2(MaxMipLevel)).xy, MaxMipLevel));
//	//float3 startPos = posT;
//	//float currHeight;
//	//float3 lastPos = startPos;
//	//int currLevel = MaxMipLevel;
//	//int2 dircSign = sign(viewRayT.xy);
//
//	//uint count = 0;
//	//while (currLevel >= 0) {
//	//	uint tWidth = exp2(MaxMipLevel - currLevel);
//	//	uint tHeight = tWidth;
//
//
//	//	int2 lastPosCellID = int2(lastPos.xy*float2(tWidth, tHeight));
//	//	currHeight = QuadDispMap.Load(int3(lastPosCellID, currLevel));
//	//	//currHeight = qdmHeightAt(lastPos.xy, int2(MaxMipLevel, currLevel), viewRayT.z);
//
//	//	if (currHeight < lastPos.z) {
//	//		float3 currPos = startPos + viewRayT * (1.f - currHeight);
//	//		int2 currPosCellID = int2(currPos.xy * float2(tWidth, tHeight));
//	//		if (currPosCellID.x != lastPosCellID.x || currPosCellID.y != lastPosCellID.y) {
//	//			float2 dlp = lastPos.xy - startPos.xy;
//	//			float2 rayingPos = (lastPosCellID + dircSign) / float2(tWidth, tHeight);
//	//			float2 drp = rayingPos - startPos.xy;
//	//			float2 dnc = abs((1.f - lastPos.z) * drp / dlp);
//	//			float currDepth = min(1.f - currHeight, min(dnc.x, dnc.y));
//	//			currHeight = 1.f - currDepth;
//	//			currLevel += 2;
//	//			currPos = startPos + viewRayT * (1.f - currHeight);
//	//		}
//	//		lastPos = currPos;
//	//		if(count > 1) return float3(0.f, 0.f, 1.f);
//	//	}
//	//	currLevel--;
//	//	count++;
//	//	if (count > 16 * MaxMipLevel)return float3(0.f, 0.f, 1.f);
//	//}
//	////float finalH = NormalMap.Sample(Sampler, lastPos.xy).w;
//	////lastPos = posT + viewRayT * stepCo * (1.f - finalH);
//	//return lastPos;
//
//
//	//float currHeight;
//	//float3 lastPos = posT;
//	//int currLevel = MaxMipLevel;
//	//int2 dircSign = sign(viewRayT.xy);
//
//	//uint count = 0;
//	//while (currLevel >= 0) {
//	//	uint tWidth = exp2(MaxMipLevel - currLevel);
//	//	int2 lastPosCellID = int2(lastPos.xy * tWidth);
//	//	currHeight = qdmHeightAt(lastPos.xy, int2(MaxMipLevel, currLevel), posT.z);
//
//	//	if (currHeight < lastPos.z) {
//	//		float3 currPos = posT + viewRayT * (posT.z - currHeight);
//	//		int2 currPosCellID = int2(currPos.xy * tWidth);
//	//		if (currPosCellID.x != lastPosCellID.x || currPosCellID.y != lastPosCellID.y) {
//	//			float2 dlp = lastPos.xy - posT.xy;
//	//			float2 rayingPos = (lastPosCellID + dircSign) / float(tWidth);
//	//			float2 drp = rayingPos - posT.xy;
//	//			float2 dnc = abs((posT.z - lastPos.z) * drp / dlp);
//	//			float currDepth = min(posT.z - currHeight, min(dnc.x, dnc.y));
//	//			currHeight = posT.z - currDepth;
//	//			currLevel += 2;
//	//			currPos = posT + viewRayT * (posT.z - currHeight);
//	//		}
//	//		lastPos = currPos;
//	//		//if(count > 4) return float3(0.f, 0.f, 1.f);
//	//	}
//	//	currLevel--;
//	//	count++;
//	//	if (count > 8 * MaxMipLevel)return float3(0.f, 0.f, 1.f);
//	//}
//	////float finalH = NormalMap.Sample(Sampler, lastPos.xy).w;
//	////lastPos = posT + viewRayT * stepCo * (1.f - finalH);
//	//return lastPos;
//
//	float currHeight;
//	float3 lastPos = posT;
//	int currLevel = MaxMipLevel;
//	int2 dircSign = sign(viewRayT.xy);
//	float maxD = 0.1f;
//	uint count = 0;
//	while (currLevel >= 0) {
//		uint tWidth = exp2(MaxMipLevel - currLevel);
//		int2 lastPosCellID = int2(lastPos.xy * tWidth);
//		currHeight = qdmHeightAt(lastPos.xy, int2(MaxMipLevel, currLevel), maxD);
//
//		if (currHeight < lastPos.z) {
//			float3 currPos = posT + viewRayT * (maxD - currHeight);
//			int2 currPosCellID = int2(currPos.xy * tWidth);
//			if (currPosCellID.x != lastPosCellID.x || currPosCellID.y != lastPosCellID.y) {
//				float2 dlp = lastPos.xy - posT.xy;
//				float2 rayingPos = (lastPosCellID + dircSign) / float(tWidth);
//				float2 drp = rayingPos - posT.xy;
//				float2 dnc = abs((maxD - lastPos.z) * drp / dlp);
//				float currDepth = min(maxD - currHeight, min(dnc.x, dnc.y));
//				currHeight = maxD - currDepth;
//				currLevel += 2;
//				currPos = posT + viewRayT * (maxD - currHeight);
//			}
//			lastPos = currPos;
//			//if(count > 0) return float3(0.f, 0.f, 1.f);
//		}
//		currLevel--;
//		count++;
//		if (count > 8 * MaxMipLevel)return float3(0.f, 0.f, 1.f);
//	}
//	//float finalH = NormalMap.Sample(Sampler, lastPos.xy).w;
//	//lastPos = posT + viewRayT * stepCo * (1.f - finalH);
//	return lastPos;
//}
float2 QDMSearch(int maxMipLevel, float3 vTangent, float2 posTex) {
	int currLevel = maxMipLevel;
	float currH;
	float3 v = float3(vTangent.xy * material.heightScale, 1.f);
	//v = v / abs(v.z);
	//v.xy *= material.texScaling;
	//v *= material.texScaling;
	//v /= abs(v.z);

	//v = v * sf;
	const float3 startPos = float3(posTex + material.heightOffset * vTangent.xy, 0.f) + v;
	//const float3 startPos = float3(posTex, 0.f);
	float3 lastPos = startPos;
	const int2 vDir = -sign(v.xy);
	int count = 0;
	while (currLevel >= 0) {
		int mipD = int(exp2(maxMipLevel - currLevel));
		currH = QuadDispMap.Load(int3(lastPos.xy * mipD, currLevel));
		int2 lastPosCID = int2(lastPos.xy * mipD);
		if (currH < lastPos.z) {
			float3 currPos = startPos - v * (1.f - currH);
			int2 currPosCID = int2(currPos.xy * mipD);
			if (currPosCID.x != lastPosCID.x || currPosCID.y != lastPosCID.y) {
				float2 lastDuv = lastPos.xy - startPos.xy;
				float2 nextPos = (vDir + lastPosCID) / float(mipD);
				float2 nextDuv = nextPos - startPos.xy;
				float2 nextD = abs((1.f - lastPos.z) * nextDuv / lastDuv);
				currH = 1.f - min(1.f - currH, min(nextD.x, nextD.y));
				currLevel += 2;
				currPos = startPos - v * (1.f - currH);
			}
			lastPos = currPos;
		}
		currLevel--;
		count++;
		if (count > 3 * maxMipLevel)
			break;
			//return float3(0.f, 0.f, 1.f);
	}

	float finalH = NormalMap.Sample(Sampler, lastPos.xy).w;
	lastPos = startPos - v * (1.f - finalH);
	return lastPos.xy;
}

float2 POSearch(float3 vTangent, float2 pTex, float2 tdx, float2 tdy) {
	//float gHeightScale = 2.f / 256.f;
	//float2 maxPaO = viewRayT.xy * gHeightScale /*/ abs(viewRayT.z)*/;
	//posTex -= maxPaO;
	//int sampleCount = 64;
	//float zStep = 1.f / (float)sampleCount;
	//float2 texStep = maxPaO * zStep;

	//int sampleI = 0;
	//float2 cTO = 0;
	//float2 lTO = 0;
	//float2 fTO = 0;
	//float cRZ = 1.f - zStep;
	//float lRZ = 1.f;
	//float cH = 0.f;
	//float lH = 0.f;

	//while (sampleI < sampleCount + 1) {
	//	cH = NormalMap.SampleGrad(LinearSampler, posTex + cTO, tdx, tdy).w;
	//	//cH = QuadDispMap.SampleGrad(LinearSampler, posTex + cTO, tdx, tdy);
	//	if (cH > cRZ) {
	//		//return (0.f, 0.f, 1.f);
	//		float t = (lH - lRZ) / (lH - cH + cRZ - lRZ);
	//		fTO = lTO + t * texStep;
	//		break;
	//	}
	//	else {
	//		sampleI++;
	//		lTO = cTO;
	//		lRZ = cRZ;
	//		lH = cH;
	//		//fTO = lTO;
	//		cTO += texStep;
	//		cRZ -= zStep;
	//	}
	//}
	//return float3(posTex + fTO, 0.f);

	float2 uvOut = pTex + material.heightOffset * vTangent.xy;
	float2 vTex = vTangent.xy * material.heightScale;

	float cH = 1.f;
	float2 cUV;
	float cHaM;

	float lH = 1.f;
	float2 lUV = uvOut + vTex;
	float lHaM = NormalMap.SampleGrad(LinearSampler, lUV, tdx, tdy).w;

	const uint stepCount = 16.f * abs(vTangent.z) + 128.f * (1 - abs(vTangent.z));

	const float dH = 1.f / float(stepCount);

	uint cStep = 0;

	while (cStep < stepCount) {
		cH -= dH;
		cUV = uvOut + vTex * cH;
		cHaM = NormalMap.SampleGrad(LinearSampler, cUV, tdx, tdy).w;

		if (cH <= cHaM) break;

		lUV = cUV;
		lH = cH;
		lHaM = cHaM;
	}

	float2 iUV = cUV + (lUV - cUV) * (cHaM - cH) / (lH - cH + cHaM - lHaM);

	return iUV;
}

float2 PMSearch(float3 viewRayT, float2 posTex) {
	float hSampled = NormalMap.Sample(LinearSampler, posTex).w;
	float h = hSampled * material.heightScale + material.heightOffset;
	return posTex -= h * viewRayT.xy;
}

float2 ISPMSearch(float3 vTangent, float2 posTex) {
	float hSampled = NormalMap.Sample(LinearSampler, posTex).w;
	float h = hSampled * material.heightScale + material.heightOffset;
	float2 tUV = posTex + h * vTangent.xy * vTangent.z;

	hSampled = NormalMap.Sample(LinearSampler, tUV).w;
	h = hSampled * material.heightScale + material.heightOffset;
	return tUV + h * vTangent.xy * vTangent.z;
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
	//float3 tangent = normalize(psi.tangentW);
	float3 tangent = normalize(psi.tangentW - dot(psi.tangentW, norm) * norm);
	float3 bitangent = normalize(cross(norm, tangent));
	float3x3 tFrameW = float3x3(tangent, bitangent, norm);
	
	float3 viewRayW = normalize(camera.posW - psi.posW);
	float3 viewRayT = mul(tFrameW, viewRayW);
	//viewRayT = normalize(viewRayT);
	//float2 adjustedPosT = psi.tex;
	//float2 adjustedPosT = QDMSearch(material.qdmMaxMipLvl, viewRayT, psi.tex);
	//float2 adjustedPosT = POSearch(viewRayT, psi.tex, ddx(psi.tex), ddy(psi.tex));
	//float2 adjustedPosT = PMSearch(viewRayT, psi.tex);
	float2 adjustedPosT = ISPMSearch(viewRayT, psi.tex);
	float3 normalT = NormalMap.Sample(LinearSampler, adjustedPosT.xy).xyz;
	normalT = normalT * 2.f - 1.f;
	float3 normW = mul(normalT, tFrameW);
	
	PSO pso;
	pso.npp = float4(
		SpheremapEncode(normW),
		mul(float4(psi.posW, 1.f), camera.mWtoV).z / camera.farClipD, 
		material.sPower);
	pso.da = DiffuseMap.Sample(Sampler, adjustedPosT.xy);

	pso.sa = float4(material.sAlbedo, 1.f);
	return pso;
}