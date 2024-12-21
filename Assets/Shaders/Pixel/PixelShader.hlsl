
CBuffer(MatBuffer, 2)
{
  float4 vTint;
  float fMult;
}

PIXEL_MAIN_BEGIN

  vec3 ambientFactor = vec3(0.3, 0.3, 0.3);  

  vec3 light = vec3(0, 0, 0);

  for (uint i = 0; i < DirLightCount; i++)
  {
    light += DirLightColor(i).xyz * max(0, dot(DirLightDir(i).xyz, normalize(inNormal))); 
  }

  vec4 color = sampleTex(Texture0, inUv);

  /*if (light < 0.5)  
  {
    color = sampleTex(gbuffTex, inUv);
  }  */

  outColor = color * vec4((light + ambientFactor), 1); 
  outColor.a = 0.7f; 

  outColor *= vTint * fMult;  
 
PIXEL_MAIN_END
