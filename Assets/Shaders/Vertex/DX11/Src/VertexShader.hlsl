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
#define buildmat3(x, y, z) transpose(float3x3(x, y, z))

//#define mul(mat, v) mul(mat, v)

#define sampleTex(tex, uv) tex.Sample(tex##_Sampler, uv)

#define Texture(name, set, bind) Texture2D name : register(t##bind);

#define CubeTexture(name, setIdx, bindIdx) TextureCube name : register(t##bind);

#define CBuffer(name, bind) cbuffer name : register(b##bind)

struct VSin
{
	float3 p : Position;
	float3 n : NORMAL;
	float3 t : TANGENT;
	float3 c : COLOR;
	float2 uv : TexCoord;
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

VERTEX_MAIN_BEGIN

	vec4 worldPos = mul(modelMat, vec4(inPos, 1.0));
	outPos = mul(viewProj, worldPos);
	outWorldPos = worldPos.xyz;
	outNormal = normalize(mul(normalMat, vec4(inNormal, 0.0))).xyz;
	outTangent = normalize(mul(normalMat, vec4(inTangent, 0.0))).xyz;
	outUv = inUv;
	
VERTEX_MAIN_END
 
