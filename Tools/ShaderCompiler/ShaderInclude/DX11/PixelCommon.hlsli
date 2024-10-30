
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

#define inPos     i.pos
#define inUv      i.uv
#define inNormal  i.normal

#define outColor  o.color

//#define ddx(x)	ddx(x)
//#define ddy(x)	ddy(x)
