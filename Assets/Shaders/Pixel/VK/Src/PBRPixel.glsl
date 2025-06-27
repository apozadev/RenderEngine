#version 450

#define lerp(a, b, t) mix(a, b, t)

#define mul(mat, v) (mat * v)

#define buildmat3(x, y, z) mat3(x, y, z)

#define sampleTex(tex, uv) texture(tex, uv)

#define sampleTexLevel(tex, uv, level) textureLod(tex, uv, level)

#define Texture(name, setIdx, bindIdx) layout(set = setIdx, binding = bindIdx) uniform sampler2D name;

#define CubeTexture(name, setIdx, bindIdx) layout(set = setIdx, binding = bindIdx) uniform samplerCube name;

#define CBuffer(name, bind) layout(set = 2, binding = bind) uniform name

#define _PI  3.1416f
#define _2PI 6.2832f
#define _PI2 1.5708f
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

CubeTexture(SpecEnvMap, 0, 5)
CubeTexture(DiffEnvMap, 0, 6)

Texture(BrdfLutTex, 0, 7)

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

// Normal distribution function (Cook-Torrance)
float _D(PBRinput _input)
{
  /*

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
  vec3 F0 = lerp(vec3(_input.reflectivity, _input.reflectivity, _input.reflectivity), _input.albedo.rgb, _input.metalness);
  float LdotH = max(dot(_input.viewDir.xyz, _input.halfVector.xyz), 0);
  return F0 + (1.0 - F0) * pow(1.0 - LdotH, 5.0);
}

// PBR lighting
vec3 PBR(PBRinput _input)
{    
  // Lambert diffuse (note: use albedo * (1 - metalness) later)
  vec3 diffuse = _input.albedo.rgb / _PI;

  // Fresnel term
  vec3 F = _F(_input);

  // Geometry (visibility) term
  float G = _G(_input);

  // Normal distribution function
  float D = _D(_input);

  // Cook-Torrance denominator
  float NdotV = max(dot(_input.normal, _input.viewDir), 0.0);
  float NdotL = max(dot(_input.normal, _input.lightDir), 0.0);
  float fsDenom = max(4.0 * NdotV * NdotL, 0.001);

  // Cook-Torrance specular
  vec3 specular = (D * G * F) / fsDenom;

  // Diffuse contribution is zero for metals
  vec3 kd = (1.0 - F) * (1.0 - _input.metalness);
  vec3 brdf = kd * diffuse + specular;

  // Incoming light intensity and cosine law
  vec3 Li = _input.lightColor.rgb;

  // Final reflected radiance
  return brdf * Li * NdotL;
}

vec3 PBRDiffuseIrradiance(PBRinput _input)
{
  vec3 F0 = lerp(vec3(_input.reflectivity, _input.reflectivity, _input.reflectivity), _input.albedo.rgb, _input.metalness);
  vec3 kd = (1.0 - F0) * (1.0 - _input.metalness);
  return kd * _input.lightColor.rgb * _input.albedo.rgb;
}

vec3 PBRSpecularIBL(PBRinput _input)
{
  vec3 kS = _FRoughness(_input);
  float NdotV = max(0, dot(_input.normal.xyz, _input.viewDir.xyz));
  vec2 envBRDF = sampleTex(BrdfLutTex, vec2(NdotV, _input.roughness)).xy;
  return (kS * envBRDF.x + envBRDF.y) * _input.lightColor.xyz;
} 

PIXEL_MAIN_BEGIN

  vec3 ambientFactor = vec3(0.3, 0.3, 0.3);  

  vec3 vN = normalize(inNormal);
  vec3 vT = normalize(inTangent - dot(inTangent, vN) * vN);
  vec3 vB = normalize(cross(inNormal, inTangent));
  mat3 mTBN = buildmat3(vT, vB, vN);

  vec3 vWN = sampleTex(Texture1, vec2(inUv.x, inUv.y)).rgb * 2.0 - 1.0;

  vWN = normalize(mul(mTBN, vWN)); 

  vec4 vAlbedo = sampleTex(Texture0, vec2(inUv.x, inUv.y));  

  vec3 vAtt = sampleTex(Texture2, vec2(inUv.x, inUv.y)).rgb;

  PBRinput pbrIn;
  pbrIn.normal = vec4(vWN, 0);
  pbrIn.viewDir = vec4(normalize(CameraPos - inWorldPos), 0);  
  pbrIn.albedo = vAlbedo;
  pbrIn.metalness = vAtt.b;
  pbrIn.roughness = vAtt.g;  
  pbrIn.reflectivity = 0.04;

  vec3 color = vec3(0,0,0);

  for (uint i = 0; i < DirLightCount; i++)
  {
    //vec4 vLightViewProjPos = mul(DirLightViewProj(i), vec4(inWorldPos, 1));

    pbrIn.lightDir = DirLightDir(i);
    pbrIn.halfVector = vec4(normalize(pbrIn.viewDir.xyz + pbrIn.lightDir.xyz), 0.0);
    pbrIn.lightColor = DirLightColor(i);// * ShadowFactor(vLightViewProjPos, i);

    color += PBR(pbrIn); 
  }

  pbrIn.lightDir = pbrIn.normal;
  pbrIn.halfVector = vec4(normalize(pbrIn.viewDir.xyz + pbrIn.lightDir.xyz), 0);
  pbrIn.lightColor = sampleTex(DiffEnvMap, -vec3(pbrIn.lightDir.x, pbrIn.lightDir.y, pbrIn.lightDir.z));

  vec3 diff = PBRDiffuseIrradiance(pbrIn);

  
  float lod = 5.f * pbrIn.roughness;
  pbrIn.lightDir = reflect(pbrIn.viewDir, pbrIn.normal);
  pbrIn.halfVector = normalize((pbrIn.viewDir + pbrIn.lightDir) * 0.5);
  pbrIn.lightColor = sampleTexLevel(SpecEnvMap, -vec3(pbrIn.lightDir.x, pbrIn.lightDir.y, pbrIn.lightDir.z), lod);

  vec3 spec = PBRSpecularIBL(pbrIn);
      
  outColor = vec4(color + diff + spec, 1); 
 
PIXEL_MAIN_END
 