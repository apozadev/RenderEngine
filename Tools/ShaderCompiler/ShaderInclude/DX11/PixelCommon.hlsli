
#define PIXEL_MAIN_BEGIN \
  struct PS_OUTPUT  \
  { \
    float4 color: SV_Target0; \
  }; \
  PS_OUTPUT main(VSout __input) { \
    PS_OUTPUT o;

#define PIXEL_MAIN_END  \
  return o; \
  } 

struct DirLight
{
  float4 vDirLightDir;
  float4 vDirLightColor;
};

cbuffer LightBuffer : register(b1)
{
  DirLight aDirLights[5];
  uint uNumLights;
};

#define inPos     __input.pos
#define inUv      __input.uv
#define inNormal  __input.normal

#define outColor  o.color

//#define ddx(x)	ddx(x)
//#define ddy(x)	ddy(x)

#define DirLightDir(i) aDirLights[i].vDirLightDir
#define DirLightColor(i) aDirLights[i].vDirLightColor
#define DirLightCount uNumLights
