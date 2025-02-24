
CBuffer(MatBuffer, 2)
{
  float fMult;
  vec4 vTint;
};

PIXEL_MAIN_BEGIN

  vec3 ambientFactor = vec3(0.3, 0.3, 0.3);  

  vec3 light = LambertDirLighting(vec4(inWorldPos, 1), inNormal);

  vec4 color = sampleTex(Texture0, float2(inUv.x, -inUv.y));  
  
  outColor = color * vec4(light + ambientFactor, 1);
  outColor.a = 0.7f; 

  outColor *= vTint * fMult; 
 
PIXEL_MAIN_END
