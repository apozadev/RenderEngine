#version 450

#define mul(mat, v) (mat * v)

#define sampleTex(tex, uv) texture(tex, uv);

#define Texture(name, setIdx, bindIdx) layout(set = setIdx, binding = bindIdx) uniform sampler2D name;
#pragma shader_stage(fragment)

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

#define PIXEL_MAIN_BEGIN \
  void main() {

#define PIXEL_MAIN_END } 

#define inPos     fragColor
#define inUv      fragTexCoord
#define inNormal  fragNormal

#define ddx(x)	dFdx(x)
#define ddy(x)	dFdy(x)

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
