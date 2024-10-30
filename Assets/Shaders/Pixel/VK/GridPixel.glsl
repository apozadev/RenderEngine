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

vec2 mod2(vec2 x, vec2 y)
{
  return x - y * floor(x / y);
}

PIXEL_MAIN_BEGIN

float scale = 99.0;
vec2 cellSize = vec2(1.0, 1.0) / scale;
vec2 halfCell = cellSize*0.5;
vec2 lineWidth = vec2(0.0001, 0.0001) / scale;

vec2 diff = fwidth(inUv);

float fade = length(diff) * scale * scale;

lineWidth += diff;

vec2 newUv = mod2(inUv, cellSize);

vec2 c = smoothstep(lineWidth, lineWidth*0.5, abs(newUv - halfCell));

float lineAlpha = max(min(c.x + c.y, 1) / fade, 0);

outColor = vec4(1, 1, 1, lineAlpha);

PIXEL_MAIN_END
