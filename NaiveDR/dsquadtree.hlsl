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

float2 PMSearch(float3 viewRayT, float2 posTex) {
	float hSampled = NormalMap.Sample(LinearSampler, posTex).w;
	float h = hSampled * material.heightScale + material.heightOffset;
	return posTex -= h * viewRayT.xy;
}

// Parallex Occlusion, Frank 2012
float2 POMSearch(float3 vTangent, float2 pTex, float2 tdx, float2 tdy) {
	float2 uvOut = pTex + material.heightOffset * vTangent.xy;
	float2 vTex = vTangent.xy * material.heightScale;

	float cH = 1.f;
	float2 cUV;
	float cHaM;

	float lH = 1.f;
	float2 lUV = uvOut + vTex;
	float lHaM = NormalMap.SampleGrad(LinearSampler, lUV, tdx, tdy).w;

	const uint stepCount = 8.f * abs(vTangent.z) + 32.f * (1.f - abs(vTangent.z));

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

	//float t = (lHaM - lH) / (lHaM - lH + cH - cHaM);
	//float2 iUV = lUV + t * vTex * dH;

	//// following a binary search
	//float3 lastPos = float3(lUV, lH);
	//float3 nextPos = float3(cUV, cH);
	//float3 midPos = (lastPos + nextPos) * 0.5f;
	//for (int i = 0; i < 5; ++i) {
	//	float3 midPos = (lastPos + nextPos) * 0.5f;
	//	float tHeight = NormalMap.SampleGrad(LinearSampler, midPos.xy, tdx, tdy).w;
	//	if (tHeight < midPos.z) {
	//		lastPos = midPos;
	//	}
	//	else {
	//		nextPos = midPos;
	//	}
	//}
	//float2 iUV = midPos.xy;

	return iUV;
}

// Iterative Sloped Parallex Mapping, Szirmay-Kalos 2008
float2 ISPMSearch(float3 vTangent, float2 posTex) {
	float hSampled = NormalMap.Sample(LinearSampler, posTex).w;
	float h = hSampled * material.heightScale + material.heightOffset;
	float2 tUV = posTex + h * vTangent.xy * vTangent.z;

	hSampled = NormalMap.Sample(LinearSampler, tUV).w;
	h = hSampled * material.heightScale + material.heightOffset;
	return tUV + h * vTangent.xy * vTangent.z;
}

// Quadtree Displacement Mapping, Michal 2010
float3 QDMSearch(int maxMipLevel, float3 vTangent, float2 pTex) {
	int currLevel = maxMipLevel;
	float3 uvhOut = float3(pTex + vTangent.xy *material.heightOffset, 0.f);
	const float3 vTex = float3(vTangent.xy * material.heightScale, 1.f);
	const int2 vDir = sign(vTex.xy);

	float currH;

	float3 lastPos = uvhOut + vTex;

	int count = 0;
	while (currLevel >= 0
		&& count <= 4 * maxMipLevel) {
		float mipD = exp2(maxMipLevel - currLevel);
		currH = QuadDispMap.Load(int3(lastPos.xy * mipD, currLevel));
		int2 lastCID = int2(lastPos.xy * mipD);

		if (currH < lastPos.z) {
			float3 currPos = uvhOut + vTex * currH;
			int2 currCID = int2(currPos.xy * mipD);

			if (currCID.x != lastCID.x 
				|| currCID.y != lastCID.y) {
				float2 lastDuv = lastPos.xy - uvhOut.xy;
				float2 nextPos = (lastCID - vDir) / mipD;
				float2 nextDuv = nextPos.xy - uvhOut.xy;
				float2 nextDz = (nextDuv / lastDuv) * lastPos.z;
				currH = max(currH, max(nextDz.x, nextDz.y));
				currLevel += 2;
				currPos = uvhOut + vTex * currH;
			}
			lastPos = currPos;
		}
		currLevel--;
		count++;
	}

	// following a binary search
	float3 midPos;
	float mipD = exp2(maxMipLevel);

	midPos = (uvhOut + lastPos) * 0.5f;
	float tHeight = QuadDispMap.Load(int3(midPos.xy * mipD, 0));
	if (tHeight < midPos.z) {
		lastPos = midPos;
	}
	else {
		uvhOut = midPos;
	}

	midPos = (uvhOut + lastPos) * 0.5f;
	tHeight = QuadDispMap.Load(int3(midPos.xy * mipD, 0));
	if (tHeight < midPos.z) {
		lastPos = midPos;
	}
	else {
		uvhOut = midPos;
	}
	midPos = (uvhOut + lastPos) * 0.5f;
	tHeight = QuadDispMap.Load(int3(midPos.xy * mipD, 0));
	if (tHeight < midPos.z) {
		lastPos = midPos;
	}
	else {
		uvhOut = midPos;
	}
	midPos = (uvhOut + lastPos) * 0.5f;
	tHeight = QuadDispMap.Load(int3(midPos.xy * mipD, 0));
	if (tHeight < midPos.z) {
		lastPos = midPos;
	}
	else {
		uvhOut = midPos;
	}
	midPos = (uvhOut + lastPos) * 0.5f;
	tHeight = QuadDispMap.Load(int3(midPos.xy * mipD, 0));
	if (tHeight < midPos.z) {
		lastPos = midPos;
	}
	else {
		uvhOut = midPos;
	}


	//for (int i = 0; i < 5; ++i) {
	//	midPos = (uvhOut + lastPos) * 0.5f;
	//	float tHeight = QuadDispMap.Load(int3(midPos.xy * mipD, 0));
	//	if (tHeight < midPos.z) {
	//		lastPos = midPos;
	//	}
	//	else {
	//		uvhOut = midPos;
	//	}
	//}
	return midPos;
	//currH = NormalMap.Sample(Sampler, lastPos.xy).w;
	//lastPos = uvhOut + vTex * currH;

	//return lastPos;

	//float mipD = exp2(maxMipLevel);
	//float dStep = 1.f / (mipD * material.heightScale);
	//lastPos += vTex * dStep * 1.5f;
	//float3 nextPos = lastPos - vTex * dStep;
	//float lastHaM = QuadDispMap.Load(int3(lastPos.xy * mipD, 0));
	//float nextHaM = QuadDispMap.Load(int3(nextPos.xy * mipD, 0));

	//float t = (lastPos.z - lastHaM) / (nextHaM - nextPos.z + lastPos.z - lastHaM);
	//return lastPos - vTex * dStep * t;
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
	;

#if defined QDM
	//float2 adjustedPosT = psi.tex;
	float3 adjustedPosT = QDMSearch(material.qdmMaxMipLvl, viewRayT, psi.tex);
#elif defined POM
	float2 adjustedPosT = POMSearch(viewRayT, psi.tex, ddx(psi.tex), ddy(psi.tex));
#else
	//float2 adjustedPosT = PMSearch(viewRayT, psi.tex);
	float2 adjustedPosT = ISPMSearch(viewRayT, psi.tex);
#endif

	//float3 normalT = NormalMap.Sample(Sampler, adjustedPosT.xy).xyz;
	float3 normalT = NormalMap.Sample(LinearSampler, adjustedPosT.xy).xyz;
	//float3 normalT = NormalMap.Load(float3(adjustedPosT.xy* exp2(material.qdmMaxMipLvl), 0.f)).xyz;
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