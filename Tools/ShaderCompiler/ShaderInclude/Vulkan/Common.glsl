#version 450

#define lerp(a, b, t) mix(a, b, t)

#define mul(mat, v) (mat * v)

#define buildmat3(x, y, z) mat3(x, y, z)

#define sampleTex(tex, uv) texture(tex, uv)

#define sampleTexLevel(tex, uv, level) textureLod(tex, uv, level)

#define Texture(name, setIdx, bindIdx) layout(set = setIdx, binding = bindIdx) uniform sampler2D name;

#define CubeTexture(name, setIdx, bindIdx) layout(set = setIdx, binding = bindIdx) uniform samplerCube name;

#define CBuffer(name, bind) layout(set = 2, binding = bind) uniform name

#define _PI  3.1416f
#define _2PI 6.2832f
#define _PI2 1.5708f
