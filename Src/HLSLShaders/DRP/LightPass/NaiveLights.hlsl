//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

Texture2D NP: register(t0);
Texture2D DA: register(t1);
Texture2D SA: register(t2);

struct LightParams
{
	float4x4 mOtoW;
	float4 pos;
	float4 color;
	float4 direction;
	float4 range;
	float2 angle;
};

cbuffer LightCB: register(b0)
{
	LightParams light;
}

struct CameraAttr {
	float4x4 mWtoV;
	float4x4 mVtoW;
	float4x4 mWtoH;
	float3 posW;
	float farClipD;
};

cbuffer CameraCB: register(b1)
{
	CameraAttr camera;
};




// Sphere map transform, Mittring 2009
float3 SpheremapDecode(float2 sm) {
	float4 nn = float4(sm, 1.f, -1.f);
	float l = dot(nn.xyz, -nn.xyw);
	nn.z = l;
	nn.xy *= sqrt(l);
	return nn.xyz * 2.f + float3(0.f, 0.f, -1.f);
	
	//float y = sqrt(1.f - sm.x*sm.x - sm.y*sm.y);
	//return float3(sm.x, y, sm.y);

	//float st;
	//float ct;
	//sincos(sm.x, st, ct);
	//float sp;
	//float cp;
	//sp = sqrt(1.f - sm.y*sm.y);
	//cp = sm.y;
	//return float3(ct*sp, st*sp, cp);
}

float3 GetViewRay(float3 posV) {
	return float3(posV.xy * (camera.farClipD / posV.z), camera.farClipD);
}

void GetGB(in float2 posH, in float3 posV,
	out float3 normW, out float3 posW,
	out float3 da, out float3 sa, out float sp)
{
	int3 indices = int3(posH, 0);
	float4 npp = NP.Load(indices);
	normW = SpheremapDecode(npp.xy);
	float3 viewRay = GetViewRay(posV);
	float4 pos = float4(viewRay * npp.z, 1.f );
	posW = mul(pos, camera.mVtoW).xyz;
	sp = npp.w;
	da = DA.Load(indices).rgb;
	sa = SA.Load(indices).rgb;
}

float3 lighting(in float3 norm, in float3 pos, in float3 da, in float3 sa, in float sp)
{
	float3 l = 0; // light vector
	float attenuation = 1.f;

#if POINTLIGHT || SPOTLIGHT
	l = light.pos.xyz - pos;
	float llength = length(l);
	attenuation = max(0, 1.f - (llength / light.range.x));
	l /= llength;
#elif DIRCTIONALLIGHT
	l = -light.direction;
#endif

#if SPOTLIGHT
	float rho = -dot(l, light.direction);
	attenuation *= saturate((rho - light.angle.y) / (light.angle.x - light.angle.y));
#endif

	float dotnl = max(0.f, dot(norm, l));
	float3 diffuse = dotnl * light.color.rgb * da;

	float3 view = normalize(camera.posW - pos);
	float3 h = normalize(l + view);
	float3 specular = pow(saturate(dot(norm, h)), sp) * light.color.rgb * sa * dotnl;

	return (specular + diffuse) * attenuation;
}

struct VSI {
	float3 posO: POSITION;
};

struct VSO {
	float4 posH : SV_POSITION;
	float3 posV : POSITION_V;
};

VSO VSMain(in VSI vsi)
{
	VSO vso;
	float4 pW = mul(float4(vsi.posO, 1.f), light.mOtoW);
	//float4 pW = float4(vsi.posO, 1.f);
	vso.posH = mul(pW, camera.mWtoH);
	vso.posV = mul(pW, camera.mWtoV).xyz;
	return vso;
}

float4 PSMain(in VSO psi) : SV_Target0
{
	float3 norm;
	float3 pos;
	float3 da;
	float3 sa;
	float sp;
	GetGB(psi.posH.xy, psi.posV, norm, pos, da, sa, sp);
	float3 lout = lighting(norm, pos, da, sa, sp);
	return float4(lout, 1.f);
//#if POINTLIGHT
//	return float4(0.5f, 0.f, 0.f, 1.f);
//#elif SPOTLIGHT
//	return float4(0.f, 0.5f, 0.f, 1.f);
//#elif DIRCTIONALLIGHT
//	return float4(0.f, 0.f, 0.5f, 1.f);
//#endif
}