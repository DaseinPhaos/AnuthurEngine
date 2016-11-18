//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

struct TexAttr {
	uint baseD;
	//uint targetD;
};

cbuffer TexAttrCB: register(b0) {
	TexAttr texAttr;
}

#if defined PS0
Texture2D<float4> BaseMap : register(t0);
#else
Texture2D<float> BaseMap : register(t0);
#endif

struct VSO {
	float4 pos: SV_POSITION;
};

VSO VSMain(in float3 vsi: POSITION_ND) {
	VSO vso;
	vso.pos = float4(vsi, 1.f);
	return vso;
}

#if defined PS0
float PSMain0(in VSO psi) : SV_Target0{
	uint2 spos = uint2(psi.pos.xy);
	return BaseMap.Load(uint3(spos, 0)).w;
}
#elif defined PS1
float PSMain1(in VSO psi) : SV_Target0{
	uint2 spos = floor(psi.pos.xy) * 2;
	//float4 samples = float4(
	//	BaseMap.Load(uint3(spos * 2, 0)),
	//	BaseMap.Load(uint3(spos.x * 2, spos.y * 2 + 1, 0)),
	//	BaseMap.Load(uint3(spos.x * 2 + 1, spos.y * 2, 0)),
	//	BaseMap.Load(uint3(spos.x * 2 + 1, spos.y * 2 + 1, 0))
	//	);
	//return max(samples.x, max(samples.y, max(samples.z, samples.w)));

	float result = BaseMap.Load(uint3(spos, 0));
	result = max(result, BaseMap.Load(uint3(spos.x + 1, spos.y, 0)));
	result = max(result, BaseMap.Load(uint3(spos.x + 1, spos.y + 1, 0)));
	result = max(result, BaseMap.Load(uint3(spos.x, spos.y + 1, 0)));
	return result;

	//float result = BaseMap.Load(uint3(spos, 0));
	//result += BaseMap.Load(uint3(spos.x + 1, spos.y, 0));
	//result += BaseMap.Load(uint3(spos.x + 1, spos.y + 1, 0));
	//result += BaseMap.Load(uint3(spos.x, spos.y + 1, 0));
	//return result/4.f;
}
#endif