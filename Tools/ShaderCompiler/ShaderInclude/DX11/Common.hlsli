#pragma pack_matrix( column_major )

#define vec2 float2
#define vec3 float3
#define vec4 float4

#define mat4 matrix
#define mat3 float3x3
#define buildmat3(x, y, z) transpose(float3x3(x, y, z))

//#define mul(mat, v) mul(mat, v)

#define sampleTex(tex, uv) tex.Sample(tex##_Sampler, uv)

#define sampleTexLevel(tex, uv, level) tex.SampleLevel(tex##_Sampler, uv, level)

#define Texture(name, set, bind) Texture2D name : register(t##bind);

#define CubeTexture(name, setIdx, bindIdx) TextureCube name : register(t##bind);

#define CBuffer(name, bind) cbuffer name : register(b##bind)

#define atan  atan2

#define _PI  3.1416f
#define _2PI 6.2832f
#define _PI2 1.5708f
