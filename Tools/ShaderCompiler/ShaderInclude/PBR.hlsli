

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
