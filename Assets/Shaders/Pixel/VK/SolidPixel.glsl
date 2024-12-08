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
    vec3 vDirLightDir;
    vec3 vDirLightColor;
};

layout(set = 1, binding = 4) uniform LightBuffer {
    DirLightData aDirLights[5];
    uint uNumLights;
};

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
#define DirLightCount uNumLighhts

PIXEL_MAIN_BEGIN

vec3 lightDir = vec3(0, 0, 1);
float ambientFactor = 0.3;

float light = max(0, dot(lightDir, normalize(inNormal)));

vec4 color = vec4(1,1,1,1);

outColor = color * (light + ambientFactor);

PIXEL_MAIN_END
