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

  float fShadow = step(fMapDepth + fBias, vProjCoords.z);
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
  float shadowFactor;
};

//Texture2D	BrdfLutTex : register(t14);
//SamplerState brdfSampler : register(s14);

// Normal distribution function
float _D(PBRinput input)
{
  float _PI = 3.14159;

  float alpha = input.roughness * input.roughness;
  float alphaSqr = alpha * alpha;
  float cosLh = max(dot(input.normal, input.halfVector), 0);
  float denom = cosLh * cosLh * (alphaSqr - 1) + 1;
  return alphaSqr / (_PI * denom * denom);//max(denominator, 0.00001);
}

// Geometry shadowing function
float _G1(vec3 N, vec3 X, float roughness)
{
  //float k = roughness * 0.5;
  float r = roughness + 1.0;
  float k = (r * r) / 0.8;
  float denominator = max(dot(N, X), 0) * (1 - k) + k;
  return max(dot(N, X), 0) / denominator;//max(denominator, 0.00001);
}
float _G(PBRinput input)
{
  return _G1(input.normal, input.lightDir, input.roughness) * _G1(input.normal, input.viewDir, input.roughness);
}

// Fresnell function
vec3 _F(PBRinput input)
{
  vec3 vF0 = input.albedo * lerp(input.reflectivity, 1.0, input.metalness);
  float LdotH = max(dot(input.viewDir, input.halfVector), 0);
  return vF0 + (vec3(1, 1, 1) - vF0) * pow(1 - LdotH, 5);
}

vec3 _FRoughness(PBRinput input)
{
  vec3 F0 = input.albedo * lerp(input.reflectivity, 1.0, input.metalness);
  vec3 invRoughness = vec3(1, 1, 1) - vec3(input.roughness, input.roughness, input.roughness);
  float LdotH = max(dot(input.viewDir, input.normal), 0);
  return F0 + (max(invRoughness, F0) - F0) * pow(1 - LdotH, 5.0);
}

// PBR lighting
vec3 PBR(PBRinput input)
{

  float _PI = 3.14159;

  // Lambert diffuse
  vec3 fd = input.albedo / _PI;

  // Cook-Torrance specular
  float fsDenom = (4 * max(dot(input.viewDir, input.normal), 0) * max(dot(input.lightDir, input.normal), 0));
  vec3 fresnell = _F(input);
  vec3 fs = _D(input) * _G(input) * fresnell / max(fsDenom, 0.00001);

  // Incoming light
  vec3 li = input.lightColor;

  // BRDF
  vec3 kd = vec3(1, 1, 1) - fresnell;
  vec3 fr = max(kd * fd + fs, vec3(0, 0, 0));

  // Cosine Law
  float lDotN = max(dot(input.lightDir, input.normal), 0);

  return fr * li * lDotN;

}

vec3 PBRDiffuseIrradiance(PBRinput input)
{
  vec3 kS = _FRoughness(input);
  vec3 kd = vec3(1, 1, 1) - kS;
  return kd * input.lightColor * input.albedo;
}

vec3 PBRSpecularIBL(PBRinput input)
{
  vec3 kS = _FRoughness(input);
  float NdotV = max(0, dot(input.normal, input.viewDir));
  //float2 envBRDF = BrdfLutTex.Sample(brdfSampler, float2(NdotV, input.roughness)).xy;
  return (kS /** envBRDF.x + envBRDF.y*/)*input.lightColor;
}

CBuffer(MatBuffer, 2)
{
  float fMult;
  vec4 vTint;
};

PIXEL_MAIN_BEGIN

  vec3 ambientFactor = vec3(0.3, 0.3, 0.3);  

  vec3 vN = normalize(inNormal);
  vec3 vT = normalize(inTangent - dot(inTangent, vN) * vN);
  vec3 vB = normalize(cross(inNormal, inTangent));
  mat3 mTBN = buildmat3(vT, vB, vN);

  vec3 vWN = sampleTex(Texture1, vec2(inUv.x, inUv.y)).rgb * 2.0 - 1.0;

  vWN = normalize(mul(mTBN, vWN)); 

  vec3 light = LambertDirLighting(vec4(inWorldPos, 1), vWN);

  vec4 color = sampleTex(Texture0, vec2(inUv.x, inUv.y));  
  
  outColor = color * vec4(light + ambientFactor, 1);
  outColor.a = 0.7f; 

  outColor *= vTint * fMult;
 
PIXEL_MAIN_END
 
