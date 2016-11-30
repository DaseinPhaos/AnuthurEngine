//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

struct CommonVSI {
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float3 tangent: TANGENT;
	float2 tex: TEXCOORD;
};

// Sphere map transform, Mittring 2009
float2 SpheremapEncode(float3 norm) {
	return normalize(norm.xy) * (sqrt(-norm.z * 0.5f + 0.5f));
}

// Sphere map transform, Mittring 2009
float3 SpheremapDecode(float2 sm) {
	float4 nn = float4(sm, 1.f, -1.f);
	float l = dot(nn.xyz, -nn.xyw);
	nn.z = l;
	nn.xy *= sqrt(l);
	return nn.xyz * 2.f + float3(0.f, 0.f, -1.f);
}

// Parallex Occlusion, Frank 2012
float2 POMSearch(float3 vTangent, float2 pTex, 
	float2 tdx, float2 tdy, 
	float heightScale, float heightOffset,
	float minSamCount, float maxSamCount,
	texture2D<float4> normalMap,
	sampler linearSampler) {
	float2 uvOut = pTex + heightOffset * vTangent.xy;
	float2 vTex = vTangent.xy * heightScale;

	float cH = 1.f;
	float2 cUV;
	float cHaM;

	float lH = 1.f;
	float2 lUV = uvOut + vTex;
	float lHaM = normalMap.SampleGrad(linearSampler, lUV, tdx, tdy).w;

	const uint stepCount = minSamCount * abs(vTangent.z) + maxSamCount * (1.f - abs(vTangent.z));

	const float dH = 1.f / float(stepCount);

	uint cStep = 0;

	while (cStep < stepCount) {
		cH -= dH;
		cUV = uvOut + vTex * cH;
		cHaM = normalMap.SampleGrad(linearSampler, cUV, tdx, tdy).w;

		if (cH <= cHaM) break;

		lUV = cUV;
		lH = cH;
		lHaM = cHaM;
	}

	float2 iUV = cUV + (lUV - cUV) * (cHaM - cH) / (lH - cH + cHaM - lHaM);

	//float t = (lHaM - lH) / (lHaM - lH + cH - cHaM);
	//float2 iUV = lUV + t * vTex * dH;

	//// alternatively, following a binary search
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
float2 ISPMSearch(float3 vTangent, float2 posTex,
	float heightScale, float heightOffset,
	texture2D<float4> normalMap,
	sampler linearSampler) {
	float hSampled = normalMap.Sample(linearSampler, posTex).w;
	float h = hSampled * heightScale + heightOffset;
	float2 tUV = posTex + h * vTangent.xy * vTangent.z;

	hSampled = normalMap.Sample(linearSampler, tUV).w;
	h = hSampled * heightScale + heightOffset;
	return tUV + h * vTangent.xy * vTangent.z;
}