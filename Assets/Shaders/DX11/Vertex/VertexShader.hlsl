

struct VSin
{
	float3 p : Position;
	float3 n : NORMAL;	
	float3 c : COLOR;	
	float2 uv : TexCoord;
};

struct VSout
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD;		
	float3 normal : NORMAL;
};

cbuffer global : register(b0)
{
	matrix viewproj;
};

cbuffer model : register(b2)
{
	matrix model;
	matrix normal;
}

VSout main(VSin input)
{
	VSout vso;

	float4 worldPos = mul(float4(input.p, 1.0), model);
	vso.pos = mul(worldPos, viewproj);	
	vso.normal = normalize(mul(float4(input.n, 0.0), normal)).xyz;		
	vso.uv = input.uv;
	return vso;
}