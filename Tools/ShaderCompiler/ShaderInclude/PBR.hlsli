
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
