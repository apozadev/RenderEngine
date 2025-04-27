#version 450

#define lerp(a, b, t) mix(a, b, t)

#define mul(mat, v) (mat * v)

#define buildmat3(x, y, z) mat3(x, y, z)

#define sampleTex(tex, uv) texture(tex, uv)

#define Texture(name, setIdx, bindIdx) layout(set = setIdx, binding = bindIdx) uniform sampler2D name;

#define CubeTexture(name, setIdx, bindIdx) layout(set = setIdx, binding = bindIdx) uniform samplerCube name;

#define CBuffer(name, bind) layout(set = 2, binding = bind) uniform name
#pragma shader_stage(geometry)

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

layout(location = 0) in vec3 fragColor[3];
layout(location = 1) in vec3 fragNormal[3];
layout(location = 2) in vec3 fragTangent[3];
layout(location = 3) in vec2 fragTexCoord[3];
layout(location = 4) in vec3 fragWorldPos[3];

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outTangent;
layout(location = 3) out vec2 outTexCoord;
layout(location = 4) out vec3 outWorldPos;

out gl_PerVertex {
    vec4 gl_Position;
};

#define inPos(i)    gl_in[i].gl_Position
#define inUv        fragTexCoord
#define inNormal    fragNormal
#define inTangent   fragTangent
#define inWorldPos  fragWorldPos

#define outPos      gl_Position

#define outLayer gl_Layer 

#define GEOM_MAIN_BEGIN \
    void main() {

#define GEOM_MAIN_END \
    }

#define BEGIN_VERTEX

#define END_VERTEX  \
    EmitVertex();

#define END_PRIMITIVE   \
    EndPrimitive();

#define SET_VERTEX_LAYER(x) \
    gl_Layer = x;

#define SET_VERTEX_POS(x)   \
    gl_Position = x;

CBuffer(ViewProjs, 2)
{
  mat4[6] aViewProjs;
};

GEOM_MAIN_BEGIN

for (int i = 0; i < 6; ++i) 
{	
	for (int j = 0; j < 3; ++j) 
	{
		BEGIN_VERTEX
		outLayer = i;
		outPos = mul(aViewProjs[i], inPos(j));
		outWorldPos = inPos(j).xyz;
		END_VERTEX
	}
	END_PRIMITIVE
}

GEOM_MAIN_END
