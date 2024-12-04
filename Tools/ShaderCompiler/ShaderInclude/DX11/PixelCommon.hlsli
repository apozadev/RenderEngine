
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

cbuffer DirLightBuffer : register(b20)
{
  float3 vDirLightDir;
  float3 vDirLightColor;
};

#define inPos     i.pos
#define inUv      i.uv
#define inNormal  i.normal

#define outColor  o.color

//#define ddx(x)	ddx(x)
//#define ddy(x)	ddy(x)

#define DirLightDir vDirLightDir
#define DirLightColor vDirLightColor
