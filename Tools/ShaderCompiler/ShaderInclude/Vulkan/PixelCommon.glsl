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
