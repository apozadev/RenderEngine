#pragma shader_stage(fragment)

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragTangent;
layout(location = 3) in vec2 fragTexCoord;
layout(location = 4) in vec3 fragWorldPos;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalBuffer {
    mat4 viewproj;
	mat4 view;
	mat4 viewInv;
	mat4 proj;
};

struct DirLightData {
    vec4 vDirLightDir;
    vec4 vDirLightColor;
};

layout(set = 1, binding = 4) uniform LightBuffer {
    DirLightData aDirLights[5];
    mat4 mLightViewProj[5];
    uint uNumLights;
    uint uNumShadows;
};

Texture(ShadowMap0, 0, 1)
Texture(ShadowMap1, 0, 2)
Texture(ShadowMap2, 0, 3)
Texture(ShadowMap3, 0, 4)

CubeTexture(Skybox, 0, 5)

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

#define inPos       fragColor
#define inUv        fragTexCoord
#define inNormal    fragNormal
#define inTangent   fragTangent
#define inWorldPos  fragWorldPos

#define ddx(x)	dFdx(x)
#define ddy(x)	dFdy(x)

#define DirLightDir(i) aDirLights[i].vDirLightDir
#define DirLightColor(i) aDirLights[i].vDirLightColor
#define DirLightViewProj(i) mLightViewProj[i]
#define DirLightCount uNumLights
#define DirLightShadowCount uNumShadows

#define CameraPos viewInv[3].xyz
