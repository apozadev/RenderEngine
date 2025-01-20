
float ShadowFactor(vec4 vLightViewProjPos, uint idx)
{
  float fShadow = 0;

  /*vec3 vProjCoords = vLightViewProjPos.xyz;
  vProjCoords.x = (vLightViewProjPos.x / vLightViewProjPos.w) * 0.5 + 0.5f;
  vProjCoords.y = 1 - ((vLightViewProjPos.y / vLightViewProjPos.w) * 0.5 + 0.5f);
  vProjCoords.z = vLightViewProjPos.z / vLightViewProjPos.w;

  if (vProjCoords.x > 1 || vProjCoords.x < 0 || vProjCoords.y > 1 || vProjCoords.y < 0)
  {
    return 1;
  }

  for (uint i = 0; i < DirLightShadowCount; i++)
  { 
    if (i == idx)
    {
      float fMapDepth = sampleTex(ShadowMap0, vProjCoords.xy).r;
      float fCurrShadow = 1 - step(fMapDepth, vProjCoords.z);
      fShadow = min(fShadow, fCurrShadow);
    }
  }*/

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
