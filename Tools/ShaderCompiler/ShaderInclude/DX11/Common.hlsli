#pragma pack_matrix( column_major )

struct VSout
{
	float4 pos : SV_Position;	
	float3 normal : NORMAL;	
	float3 worldPos : WORLDPOS;
	float2 uv : TEXCOORD;
	float3 tangent : TANGENT;
};

#define vec2 float2
#define vec3 float3
#define vec4 float4

#define mat4 matrix
#define mat3 float3x3

#define mul(mat, v) mul(mat, v)

#define sampleTex(tex, uv) tex.Sample(tex##_Sampler, uv)

#define Texture(name, set, bind) Texture2D name : register(t##bind);

#define CBuffer(name, bind) cbuffer name : register(b##bind)
