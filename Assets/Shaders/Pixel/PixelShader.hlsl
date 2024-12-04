
Texture(gbuffTex,1,0)

Texture(albedoTex,3,0)

PIXEL_MAIN_BEGIN

  vec3 ambientFactor = vec3(0.3, 0.3, 0.3);  

  vec3 light = DirLightColor * max(0, dot(DirLightDir, normalize(inNormal)));

  vec4 color = sampleTex(albedoTex, inUv);

  /*if (light < 0.5)  
  {
    color = sampleTex(gbuffTex, inUv);
  }  */

  outColor = color * vec4((light + ambientFactor), 1);
  outColor.a = 0.7f; 
 
PIXEL_MAIN_END
