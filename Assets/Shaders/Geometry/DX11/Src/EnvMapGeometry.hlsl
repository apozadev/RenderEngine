#pragma pack_matrix( column_major )

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

#define atan  atan2

struct GSin
{
	float4 pos : SV_Position;
	float3 normal : NORMAL;
	float3 worldPos : WORLDPOS;
	float2 uv : TEXCOORD;
	float3 tangent : TANGENT;
};

struct GSout
{
	float4 pos : SV_Position;
	float3 normal : NORMAL;
	float3 worldPos : WORLDPOS;
	float2 uv : TEXCOORD;
	float3 tangent : TANGENT;
	uint targetIdx : SV_RenderTargetArrayIndex;
};

#define inPos(i)    _input[i].pos
#define inUv        _input[i].uv
#define inNormal    _input[i].normal
#define inTangent   _input[i].tangent
#define inWorldPos  _input[i].worldPos

#define outPos      element.pos
#define outWorldPos	element.worldPos
#define outLayer		element.targetIdx

#define GEOM_MAIN_BEGIN	\
[maxvertexcount(18)]	\
void main(	\
	triangle GSin _input[3],	\
	inout TriangleStream<GSout> _output	\
)	\
{	\
GSout element;	\
element.normal = float3(0,0,0);	\
element.worldPos = float3(0,0,0);	\
element.uv = float2(0,0);	\
element.tangent = float3(0,0,0);	\
element.targetIdx = 0;	

#define GEOM_MAIN_END	}

#define END_VERTEX  \
    _output.Append(element);

#define END_PRIMITIVE   \
    _output.RestartStrip();

CBuffer(ViewProjs, 2)
{
  mat4 aViewProjs[6];
};

GEOM_MAIN_BEGIN

for (int i = 0; i < 6; ++i) 
{	
	for (int j = 0; j < 3; ++j) 
	{
		outLayer = i;
		outPos = mul(aViewProjs[i], inPos(j));
		outWorldPos = inPos(j).xyz;
		END_VERTEX
	}
	END_PRIMITIVE
}

GEOM_MAIN_END

