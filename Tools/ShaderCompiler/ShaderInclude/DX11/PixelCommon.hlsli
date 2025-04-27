
struct PSin
{
	float4 pos : SV_Position;
	float3 normal : NORMAL;
	float3 worldPos : WORLDPOS;
	float2 uv : TEXCOORD;
	float3 tangent : TANGENT;
};

#define PIXEL_MAIN_BEGIN \
  struct PS_OUTPUT  \
  { \
    float4 color: SV_Target0; \
  }; \
  PS_OUTPUT main(PSin __input) { \
    PS_OUTPUT o;

#define PIXEL_MAIN_END  \
  return o; \
  } 

cbuffer GlobalBuffer : register(b0)
{
  matrix viewproj;
  matrix view;
  matrix viewInv;
  matrix proj;
};

struct DirLight
{
  float4 vDirLightDir;
  float4 vDirLightColor;
};

CBuffer(LightBuffer, 1)
{
  DirLight aDirLights[5];  
  mat4 mLightViewProj[5];
  uint uNumLights;
  uint uNumShadows;
}

Texture2D Input0 : register(t0);
Texture2D Input1 : register(t1);
Texture2D Input2 : register(t2);
Texture2D Input3 : register(t3);

Texture2D Texture0 : register(t4);
Texture2D Texture1 : register(t5);
Texture2D Texture2 : register(t6);
Texture2D Texture3 : register(t7);

Texture2D ShadowMap0 : register(t8);
Texture2D ShadowMap1 : register(t9);
Texture2D ShadowMap2 : register(t10);
Texture2D ShadowMap3 : register(t11);

TextureCube Skybox : register(t12);

sampler Input0_Sampler : register(s0);
sampler Input1_Sampler : register(s1);
sampler Input2_Sampler : register(s2);
sampler Input3_Sampler : register(s3);

sampler Texture0_Sampler : register(s4);
sampler Texture1_Sampler : register(s5);
sampler Texture2_Sampler : register(s6);
sampler Texture3_Sampler : register(s7);

sampler ShadowMap0_Sampler : register(s8);
sampler ShadowMap1_Sampler : register(s9);
sampler ShadowMap2_Sampler : register(s10);
sampler ShadowMap3_Sampler : register(s11);

sampler Skybox_Sampler : register(s12);

#define viewProj	viewproj

#define inPos       __input.pos
#define inUv        __input.uv
#define inNormal    __input.normal
#define inTangent   __input.tangent
#define inWorldPos  __input.worldPos

#define outColor  o.color

//#define ddx(x)	ddx(x)
//#define ddy(x)	ddy(x)

#define DirLightDir(i) aDirLights[i].vDirLightDir
#define DirLightColor(i) aDirLights[i].vDirLightColor
#define DirLightCount uNumLights
#define DirLightViewProj(i) mLightViewProj[i]
#define DirLightCount uNumLights
#define DirLightShadowCount uNumShadows

#define CameraPos float3(viewInv._14, viewInv._24, viewInv._34)
