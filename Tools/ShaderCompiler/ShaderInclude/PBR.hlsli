

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
  vec3 F0 = _input.albedo.xyz * lerp(_input.reflectivity, 1.0, _input.metalness);
  vec3 invRoughness = vec3(1, 1, 1) - vec3(_input.roughness, _input.roughness, _input.roughness);
  float LdotH = max(dot(_input.viewDir.xyz, _input.halfVector.xyz), 0);
  return F0 + (max(invRoughness, F0) - F0) * pow(1 - LdotH, 5.0);
}

// PBR lighting
vec3 PBR(PBRinput _input)
{    

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
