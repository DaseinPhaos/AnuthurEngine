cbuffer cbScene : register(b0)
{
	float4x4 gWH;
};
cbuffer sbObject : register(b1)
{
	float4x4 gOW;
};
struct VSIn {
	float4 pos: POSITION;
	float4 color : COLOR;
};
struct VSOut {
	float4 pos: SV_POSITION;
	float4 color: COLOR;
};


VSOut VS(VSIn vin) {
	float4 pW = mul(vin.pos, gOW);
	VSOut vout;
	vout.pos = mul(pW, gWH);
	vout.color = vin.color;
	return vout;
}

float4 PS(VSOut pin) :SV_TARGET{
	return pin.color;
}