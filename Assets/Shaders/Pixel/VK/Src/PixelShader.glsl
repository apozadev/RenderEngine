#version 450

#define lerp(a, b, t) mix(a, b, t)

#define mul(mat, v) (mat * v)

#define buildmat3(x, y, z) mat3(x, y, z)

#define sampleTex(tex, uv) texture(tex, uv)

#define Texture(name, setIdx, bindIdx) layout(set = setIdx, binding = bindIdx) uniform sampler2D name;

#define CubeTexture(name, setIdx, bindIdx) layout(set = setIdx, binding = bindIdx) uniform samplerCube name;

#define CBuffer(name, bind) layout(set = 2, binding = bind) uniform name
#pragma shader_stage(fragment)

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragTangent;
layout(location = 3) in vec2 fragTexCoord;
layout(location = 4) in vec3 fragWorldPos;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalBuffer {
    mat4 viewproj;
	mat4 view;
	mat4 viewInv;
	mat4 proj;
};

struct DirLightData {
    vec4 vDirLightDir;
    vec4 vDirLightColor;
};

layout(set = 1, binding = 4) uniform LightBuffer {
    DirLightData aDirLights[5];
    mat4 mLightViewProj[5];
    uint uNumLights;
    uint uNumShadows;
};

Texture(ShadowMap0, 0, 1)
Texture(ShadowMap1, 0, 2)
Texture(ShadowMap2, 0, 3)
Texture(ShadowMap3, 0, 4)

CubeTexture(Skybox, 0, 5)

Texture(Input0, 1, 0)
Texture(Input1, 1, 1)
Texture(Input2, 1, 2)
Texture(Input3, 1, 3)

Texture(Texture0, 3, 0)
Texture(Texture1, 3, 1)
Texture(Texture2, 3, 2)
Texture(Texture3, 3, 3)

#define PIXEL_MAIN_BEGIN \
  void main() {

#define PIXEL_MAIN_END } 

#define inPos       fragColor
#define inUv        fragTexCoord
#define inNormal    fragNormal
#define inTangent   fragTangent
#define inWorldPos  fragWorldPos

#define ddx(x)	dFdx(x)
#define ddy(x)	dFdy(x)

#define DirLightDir(i) aDirLights[i].vDirLightDir
#define DirLightColor(i) aDirLights[i].vDirLightColor
#define DirLightViewProj(i) mLightViewProj[i]
#define DirLightCount uNumLights
#define DirLightShadowCount uNumShadows

#define CameraPos viewInv[3].xyz

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

// Normal distribution function
float _D(PBRinput _input)
{
  float _PI = 3.14159;

  float alpha = _input.roughness * _input.roughness;
  float alphaSqr = alpha * alpha;
  float cosLh = max(dot(_input.normal.xyz, _input.halfVector.xyz), 0);
  float denom = cosLh * cosLh * (alphaSqr - 1) + 1;
  return alphaSqr / max(_PI * denom * denom, 0.0001);
}

// Geometry shadowing function
float _G1(vec4 N, vec4 X, float roughness)
{
  //float k = roughness * 0.5;
  float r = roughness + 1.0;
  float k = (r * r) / 0.5;
  float denominator = max(dot(N.xyz, X.xyz), 0) * (1 - k) + k;
  return max(dot(N.xyz, X.xyz), 0) / denominator;//max(denominator, 0.00001);
}
float _G(PBRinput _input)
{
  return _G1(_input.normal, _input.lightDir, _input.roughness) * _G1(_input.normal, _input.viewDir, _input.roughness);
}

// Fresnell function
vec3 _F(PBRinput _input) 
{
  vec3 vF0 = _input.albedo.xyz * lerp(_input.reflectivity, 1.0, _input.metalness);
  float LdotH = max(dot(_input.viewDir, _input.halfVector), 0);
  return vF0 + (vec3(1, 1, 1) - vF0) * pow(1 - LdotH, 5);
}

vec3 _FRoughness(PBRinput _input)
{
  vec3 F0 = _input.albedo.xyz * lerp(_input.reflectivity, 1.0, _input.metalness);
  vec3 invRoughness = vec3(1, 1, 1) - vec3(_input.roughness, _input.roughness, _input.roughness);
  float LdotH = max(dot(_input.viewDir.xyz, _input.normal.xyz), 0);
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
  vec3 kd = mix(vec3(1, 1, 1) - fresnell, vec3(0, 0, 0), _input.metalness);
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
 