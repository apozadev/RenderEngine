
vec2 mod2(vec2 x, vec2 y)
{
  return x - y * floor(x / y);
}

PIXEL_MAIN_BEGIN

float scale = 99.0;
vec2 cellSize = vec2(1.0, 1.0) / scale;
vec2 halfCell = cellSize * 0.5;
vec2 lineWidth = vec2(0.0001, 0.0001) / scale;

vec2 diff = fwidth(inUv);

float fade = length(diff) * scale * scale;

lineWidth += diff * 0.8;

vec2 newUv = mod2(inUv, cellSize);

vec2 c = smoothstep(lineWidth, lineWidth * 0.5, abs(newUv - halfCell));

float lineAlpha = 1 - max(min(c.x + c.y, 1) / fade, 0);

outColor = vec4(lineAlpha, lineAlpha, lineAlpha, 1);

vec3 ambientFactor = vec3(0.3, 0.3, 0.3);

outColor = outColor * (vec4(LambertDirLighting(vec4(inWorldPos, 1), inNormal) + ambientFactor, 1));

PIXEL_MAIN_END 
