
Texture(gbuffTex,1,0)

Texture(albedoTex,3,0)

PIXEL_MAIN_BEGIN

  vec3 lightDir = vec3(0, 0, 1);
  float ambientFactor = 0.3;  

  float light = max(0, dot(lightDir, normalize(inNormal)));

  vec4 color = sampleTex(albedoTex, inUv);

  if (light < 0.5)
  {
    color = sampleTex(gbuffTex, inUv);
  }  

  outColor = color * (light + ambientFactor);
  outColor.a = 0.7f; 
 
PIXEL_MAIN_END
