
struct VSin
{
	float3 p : Position;
	float3 n : NORMAL;
	float3 t : TANGENT;
	float3 c : COLOR;
	float2 uv : TexCoord;
};

struct VSout
{
	float4 pos : SV_Position;
	float3 normal : NORMAL;
	float3 worldPos : WORLDPOS;
	float2 uv : TEXCOORD;
	float3 tangent : TANGENT;
};

cbuffer GlobalBuffer : register(b0)
{
	matrix viewproj;
	matrix view;
	matrix viewInv;
	matrix proj;
};

cbuffer ModelBuffer : register(b3)
{
	matrix model;
	matrix normal;
}

#define VERTEX_MAIN_BEGIN \
	VSout main(VSin input) { \
		VSout vso;

#define VERTEX_MAIN_END \
		return vso; \
	}

#define viewProj	viewproj
#define modelMat	model
#define normalMat normal

#define inPos			input.p
#define inNormal	input.n
#define inTangent	input.t
#define inColor		input.c
#define inUv			input.uv

#define outPos			vso.pos
#define outNormal		vso.normal
#define outTangent	vso.tangent
#define outUv				vso.uv
#define outWorldPos	vso.worldPos
