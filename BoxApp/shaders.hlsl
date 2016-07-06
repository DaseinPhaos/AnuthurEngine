cbuffer Matrix44 : register(b0)
{
	float4x4 WHmatrix;
}

struct Vertex
{
	float4 pos : POSITION;
	float4 color : COLOR;
};

struct VS_Output
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

VS_Output VS(Vertex vin)
{
	VS_Output vout;
	vout.pos = mul( vin.pos, WHmatrix);
	vout.color = vin.color;
	return vout;
}

float4 PS(VS_Output v): SV_Target
{
	return v.color;
}