
struct VSin
{
	float3 p : Position;
	float3 n : NORMAL;
	float3 c : COLOR;
	float2 uv : TexCoord;
};

cbuffer GlobalBuffer : register(b0)
{
	matrix viewproj;
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
#define inColor		input.c
#define inUv			input.uv

#define outPos		vso.pos
#define outNormal vso.normal
#define outUv			vso.uv
