//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

Texture2D Norm: register(t0);
Texture2D Da: register(t1);
Texture2D Sa: register(t2);
Texture2D Pos: register(t3);

struct LightParams
{
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

cbuffer CameraCB : register(b1)
{
	float3 cameraPos;
}

struct gbOutput {
	float3 norm;
	float3 pos;
	float3 da;
	float3 sa;
	float sp;
};

void getGB(in float2 screenPos,
	out float3 norm, out float3 pos, out float3 da, out float3 sa, out float sp)
{
	int3 indices = int3(screenPos.xy, 0);
	norm = Norm.Load(indices).xyz;
	pos = Pos.Load(indices).xyz;
	da = Da.Load(indices).xyz;
	float4 s = Sa.Load(indices);
	sa = s.xyz;
	sp = s.w;
}

//gbOutput getGB(float2 screenPos) {
//	int3 indices = int3(screenPos.xy, 0);
//	gbOutput o;
//	o.norm = Norm.Load(indices).xyz;
//	o.pos = Pos.Load(indices).xyz;
//	o.da = Da.Load(indices).xyz;
//	float4 s = Sa.Load(indices);
//	o.sa = s.xyz;
//	o.sp = s.w;
//	return o;
//}

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

	float3 view = normalize(cameraPos - pos);
	float3 h = normalize(l + view);
	float3 specular = pow(saturate(dot(norm, h)), sp) * light.color.rgb * sa * dotnl;

	return (specular + diffuse) * attenuation;
}

struct VSI
{
	float3 pos: POSITION;
};

float4 VS(in VSI vsi) :SV_Position
{
	return float4(vsi.pos, 1.f);
}

float4 PS(in float4 screenPos: SV_Position) : SV_Target0
{
	float3 norm;
	float3 pos;
	float3 da;
	float3 sa;
	float sp;
	getGB(screenPos.xy, norm, pos, da, sa, sp);
	float3 lout = lighting(norm, pos, da, sa, sp);
	//if (screenPos.x < 1000.f && screenPos.y < 1000.f) {
	//	lout += float3(0.5f, 0.f, 0.f);
	//}
	//lout += float3(0.5f, 0.f, 0.f);
	return float4(lout, 1.f);
}