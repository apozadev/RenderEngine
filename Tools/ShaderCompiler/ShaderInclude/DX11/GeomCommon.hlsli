
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
