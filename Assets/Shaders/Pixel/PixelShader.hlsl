
Texture(albedoTex, 3, 0)

PIXEL_MAIN_BEGIN

  vec3 lightDir = vec3(0, 0, 1);
  float ambientFactor = 0.3;

  vec4 color = sampleTex(albedoTex, inUv);  

  outColor = color * (max(0, dot(lightDir, normalize(inNormal))) + ambientFactor);
  outColor.a = 0.7f; 
 
PIXEL_MAIN_END
