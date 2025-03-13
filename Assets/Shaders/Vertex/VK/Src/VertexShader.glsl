#version 450

#define lerp(a, b, t) mix(a, b, t)

#define mul(mat, v) (mat * v)

#define buildmat3(x, y, z) mat3(x, y, z)

#define sampleTex(tex, uv) texture(tex, uv)

#define Texture(name, setIdx, bindIdx) layout(set = setIdx, binding = bindIdx) uniform sampler2D name;

#define CubeTexture(name, setIdx, bindIdx) layout(set = setIdx, binding = bindIdx) uniform samplerCube name;

#define CBuffer(name, bind) layout(set = 2, binding = bind) uniform name
#pragma shader_stage(vertex)

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec3 inColor;
layout(location = 4) in vec2 inUv;

layout(set = 0, binding = 0) uniform GlobalBuffer {
    mat4 viewproj;
	mat4 view;
	mat4 viewInv;
	mat4 proj;
};

layout( push_constant ) uniform constants  
{	
	mat4 model;
	mat4 normal;
} PushConstants;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragTangent;
layout(location = 3) out vec2 fragTexCoord;
layout(location = 4) out vec3 fragWorldPos;

#define VERTEX_MAIN_BEGIN \
    void main() {

#define VERTEX_MAIN_END \
	}

#define viewProj	viewproj
#define modelMat	PushConstants.model
#define normalMat   PushConstants.normal

#define inPos	    inPosition
//#define inNormal	inNormal
//#define inTangent inTangent
//#define inColor	inColor
//#define inUv		inUv

#define outPos		gl_Position
#define outNormal   fragNormal
#define outTangent	fragTangent
#define outColor   	fragColor
#define outUv		fragTexCoord
#define outWorldPos fragWorldPos


VERTEX_MAIN_BEGIN

	vec4 worldPos = mul(modelMat, vec4(inPos, 1.0));
	outPos = mul(viewProj, worldPos);
	outWorldPos = worldPos.xyz;
	outNormal = normalize(mul(normalMat, vec4(inNormal, 0.0))).xyz;
	outTangent = normalize(mul(normalMat, vec4(inTangent, 0.0))).xyz;
	outUv = inUv;
	
VERTEX_MAIN_END
 