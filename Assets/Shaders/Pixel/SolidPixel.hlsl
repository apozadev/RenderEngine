
PIXEL_MAIN_BEGIN

vec3 lightDir = vec3(0, 0, 1);
float ambientFactor = 0.3;

float light = max(0, dot(lightDir, normalize(inNormal)));

vec4 color = vec4(1,1,1,1);

outColor = color * (light + ambientFactor); 

PIXEL_MAIN_END
