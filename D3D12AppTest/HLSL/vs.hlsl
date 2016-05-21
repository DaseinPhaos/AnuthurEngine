cbuffer wpMatrix : register(b0)
{
	float4x4 gWPTransform;
};

struct VSInput
{
	float4 pos: POSITION;
	float4 color : COLOR;
};

struct VSOutput
{
	float4 pos: SV_POSITION;
	float4 color : COLOR;
};

VSOutput VS(VSInput v) {
	VSOutput vout;
	vout.pos = mul(gWPTransform, v.pos);
	vout.color = v.color;
	return vout;
}