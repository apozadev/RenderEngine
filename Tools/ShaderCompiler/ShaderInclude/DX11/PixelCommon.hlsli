
#define PIXEL_MAIN_BEGIN \
  struct PS_OUTPUT  \
  { \
    float4 color: SV_Target0; \
  }; \
  PS_OUTPUT main(VSout i) { \
    PS_OUTPUT o;

#define PIXEL_MAIN_END  \
  return o; \
  } 

struct DirLight
{
  float3 vDirLightDir;
  float3 vDirLightColor;
};

cbuffer LightBuffer : register(b20)
{
  DirLight aDirLights[5];
  uint uNumLights;
};

#define inPos     i.pos
#define inUv      i.uv
#define inNormal  i.normal

#define outColor  o.color

//#define ddx(x)	ddx(x)
//#define ddy(x)	ddy(x)

#define DirLightDir(i) aDirLights[i].vDirLightDir
#define DirLightColor(i) aDirLights[i].vDirLightColor
#define DirLightCount uNumLights
