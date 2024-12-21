#version 450

#define mul(mat, v) (mat * v)

#define sampleTex(tex, uv) texture(tex, uv);

#define Texture(name, setIdx, bindIdx) layout(set = setIdx, binding = bindIdx) uniform sampler2D name;
#pragma shader_stage(fragment)

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

struct DirLightData {
    vec4 vDirLightDir;
    vec4 vDirLightColor;
};

layout(set = 1, binding = 4) uniform LightBuffer {
    DirLightData aDirLights[5];
    uint uNumLights;
};

Texture(Input0, 1, 0)
Texture(Input1, 1, 1)
Texture(Input2, 1, 2)
Texture(Input3, 1, 3)

Texture(Texture0, 3, 0)
Texture(Texture1, 3, 1)
Texture(Texture2, 3, 2)
Texture(Texture3, 3, 3)

#define PIXEL_MAIN_BEGIN \
  void main() {

#define PIXEL_MAIN_END } 

#define inPos     fragColor
#define inUv      fragTexCoord
#define inNormal  fragNormal

#define ddx(x)	dFdx(x)
#define ddy(x)	dFdy(x)

#define DirLightDir(i) aDirLights[i].vDirLightDir
#define DirLightColor(i) aDirLights[i].vDirLightColor
#define DirLightCount uNumLights
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
 
PIXEL_MAIN_END
