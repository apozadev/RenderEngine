#pragma pack_matrix( column_major )

struct VSout
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
};

SamplerState texSampler : register(s0);

#define vec2 float2
#define vec3 float3
#define vec4 float4

#define mat4 matrix

#define mul(mat, v) mul(mat, v)

#define sampleTex(tex, uv) tex.Sample(texSampler, uv)

#define Texture(name, set, bind) Texture2D name : register(t##bind);