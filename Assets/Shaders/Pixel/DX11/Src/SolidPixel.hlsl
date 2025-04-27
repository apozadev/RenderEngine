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

float ShadowFactor(vec4 vLightViewProjPos, uint idx)
{
  vec3 vProjCoords = vLightViewProjPos.xyz;
  vProjCoords.x = (vLightViewProjPos.x / vLightViewProjPos.w) * 0.5 + 0.5f;
  vProjCoords.y = 1 - ((vLightViewProjPos.y / vLightViewProjPos.w) * 0.5 + 0.5f);
  vProjCoords.z = vLightViewProjPos.z / vLightViewProjPos.w;

  if (vProjCoords.x > 1 || vProjCoords.x < 0 || vProjCoords.y > 1 || vProjCoords.y < 0)
  {
    return 1;
  }  

  float fMapDepth = 0;

  if (idx == 0)
  {
    fMapDepth = sampleTex(ShadowMap0, vProjCoords.xy).r;
  }
  else if (idx == 1)
  {
    fMapDepth = sampleTex(ShadowMap1, vProjCoords.xy).r;
  }
  else if (idx == 2)
  {
    fMapDepth = sampleTex(ShadowMap2, vProjCoords.xy).r;
  }
  else if (idx == 3)
  {
    fMapDepth = sampleTex(ShadowMap3, vProjCoords.xy).r;
  }

  float fBias = 0.00002f;

  float fShadow = step(fMapDepth /*+ fBias*/, vProjCoords.z);
  fShadow = 1 - max(0, min(1, fShadow));

  return fShadow;
}

vec3 LambertDirLighting(vec4 vWorldPos, vec3 vNormal)
{
  vec3 vLight = vec3(0, 0, 0);

  for (uint i = 0; i < DirLightCount; i++)
  {
    vec4 vLightViewProjPos = mul(DirLightViewProj(i), vWorldPos);
    vLight += DirLightColor(i).xyz * max(0, dot(DirLightDir(i).xyz, normalize(vNormal))) * ShadowFactor(vLightViewProjPos, i);
  }

  return vLight;
}

/*
float ShadowFactor(vec4 vLightViewProjPos, uint idx)
{
  float fShadow = 0;

  vec3 vProjCoords = vLightViewProjPos.xyz;
  vProjCoords.x = (vLightViewProjPos.x / vLightViewProjPos.w) * 0.5 + 0.5f;
  vProjCoords.y = 1 - ((vLightViewProjPos.y / vLightViewProjPos.w) * 0.5 + 0.5f);
  vProjCoords.z = vLightViewProjPos.z / vLightViewProjPos.w;

  if (vProjCoords.x > 1 || vProjCoords.x < 0 || vProjCoords.y > 1 || vProjCoords.y < 0)
  {
    return 1;
  }

  //for (uint i = 0; i < DirLightShadowCount; i++)
  //{ 
  //  if (i == idx)
  //  {
  //    float fMapDepth = sampleTex(ShadowMap0, vProjCoords.xy).r;
  //    float fCurrShadow = 1 - step(fMapDepth, vProjCoords.z);
  //    fShadow = min(fShadow, fCurrShadow);
  //  }
  //}

return vProjCoords.z;
}

vec3 LambertDirLighting(vec4 vWorldPos, vec3 vNormal)
{
  vec3 vLight = vec3(0, 0, 0);

  //for (uint i = 0; i < DirLightCount; i++)
  //{
  //  vec4 vLightViewProjPos = mul(DirLightViewProj(i), vWorldPos);
  //  vLight += DirLightColor(i).xyz * max(0, dot(DirLightDir(i).xyz, normalize(vNormal))) * ShadowFactor(vLightViewProjPos, i);
  //}

  float t = ShadowFactor(vLightViewProjPos, i);

  vLight = vec3(1, 0, 0) * t + vec3(0, 1, 0) * (1 - t);

  return vLight;
}
*/


//Texture2D	BrdfLutTex : register(t14);
//SamplerState brdfSampler : register(s14);

struct PBRinput
{
  vec4 normal;
  vec4 viewDir;
  vec4 halfVector;
  vec4 lightDir;
  vec4 lightColor;
  vec4 albedo;
  vec4 emissive;
  float roughness;
  float metalness;
  float reflectivity;
};

// Normal distribution function (Cook-Torrance)
float _D(PBRinput _input)
{
  /*float _PI = 3.14159;

  float alpha2 = _input.roughness * _input.roughness * _input.roughness * _input.roughness;
  float NdotH = max(dot(_input.normal.xyz, _input.halfVector.xyz), 0);
  float d = NdotH * NdotH * (alpha2 - 1) + 1;
  return alpha2 / (_PI * d * d);*/

  float PI = 3.14159;

  float cosLh = max(dot(_input.normal.xyz, _input.halfVector.xyz), 0);

  float alpha = _input.roughness * _input.roughness;
  float alphaSq = alpha * alpha;

  float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
  return alphaSq / (PI * denom * denom);
}

// Blinn-Phong
float _DBlinn(PBRinput _input)
{
  float _PI = 3.14159;

  float fAlphaSqr = _input.roughness * _input.roughness * _input.roughness * _input.roughness;
  float fDenom = _PI * fAlphaSqr;
  float fPower = (2.0 / fAlphaSqr) - 2.0;
  float fHDotN = dot(_input.halfVector.xyz, _input.normal.xyz);
  return pow(fHDotN, fPower) / fDenom;
}

// Geometry shadowing function
float _G1(vec4 N, vec4 X, float roughness)
{
  float r = roughness + 1.0;
  float k = (r * r) / 8.0;
  float denominator = max(dot(N.xyz, X.xyz), 0) * (1 - k) + k;
  return max(dot(N.xyz, X.xyz), 0) / denominator;
}
float _G(PBRinput _input)
{
  return _G1(_input.normal, _input.lightDir, _input.roughness) * _G1(_input.normal, _input.viewDir, _input.roughness);
}

// Fresnell function
vec3 _F(PBRinput _input) 
{
  vec3 vF0 = vec3(_input.reflectivity, _input.reflectivity, _input.reflectivity);
  vF0 = lerp(vF0, _input.albedo.xyz, _input.metalness);
  float LdotH = max(dot(_input.viewDir, _input.halfVector), 0);
  return vF0 + (vec3(1, 1, 1) - vF0) * pow(1 - LdotH, 5);
}

vec3 _FRoughness(PBRinput _input)
{
  vec3 F0 = _input.albedo.xyz * lerp(_input.reflectivity, 1.0, _input.metalness);
  vec3 invRoughness = vec3(1, 1, 1) - vec3(_input.roughness, _input.roughness, _input.roughness);
  float LdotH = max(dot(_input.viewDir.xyz, _input.halfVector.xyz), 0);
  return F0 + (max(invRoughness, F0) - F0) * pow(1 - LdotH, 5.0);
}

// PBR lighting
vec3 PBR(PBRinput _input)
{
  
  float _PI = 3.14159;

  // Lambert diffuse
  vec3 fd = _input.albedo.xyz / _PI;

  // Cook-Torrance specular
  float fsDenom = (4 * max(dot(_input.viewDir.xyz, _input.normal.xyz), 0) * max(dot(_input.lightDir.xyz, _input.normal.xyz), 0));
  vec3 fresnell = _F(_input);
  vec3 fs = _D(_input) * _G(_input) * fresnell / max(fsDenom, 0.0001);

  // Incoming light
  vec3 li = _input.lightColor.xyz;

  // BRDF
  //vec3 kd = lerp(vec3(1, 1, 1) - fresnell, vec3(0, 0, 0), _input.metalness);
  vec3 kd = vec3(1, 1, 1) - fresnell;
  vec3 fr = max(kd * fd + fs, vec3(0, 0, 0));

  // Cosine Law
  float lDotN = max(dot(_input.lightDir.xyz, _input.normal.xyz), 0);

  return fr * li * lDotN;
  
}

vec3 PBRDiffuseIrradiance(PBRinput _input)
{
  vec3 kS = _FRoughness(_input);
  vec3 kd = vec3(1, 1, 1) - kS;
  return kd * _input.lightColor.xyz * _input.albedo.xyz;
}

vec3 PBRSpecularIBL(PBRinput _input)
{
  vec3 kS = _FRoughness(_input);
  float NdotV = max(0, dot(_input.normal.xyz, _input.viewDir.xyz));
  //float2 envBRDF = BrdfLutTex.Sample(brdfSampler, float2(NdotV, _input.roughness)).xy;
  return (kS /** envBRDF.x + envBRDF.y*/) * _input.lightColor.xyz;
}

PIXEL_MAIN_BEGIN

vec3 lightDir = vec3(0, 0, 1);
float ambientFactor = 0.3;

float light = max(0, dot(lightDir, normalize(inNormal)));

vec4 color = vec4(1,1,1,1);

outColor = color * (light + ambientFactor); 

PIXEL_MAIN_END 

