#pragma shader_stage(vertex)

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inUv;

layout(set = 0, binding = 0) uniform GlobalBuffer {
    mat4 viewproj;
};

layout( push_constant ) uniform constants  
{	
	mat4 model;
	mat4 normal;
} PushConstants;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;

#define VERTEX_MAIN_BEGIN \
    void main() {

#define VERTEX_MAIN_END \
	}

#define viewProj	viewproj
#define modelMat	PushConstants.model
#define normalMat   PushConstants.normal

#define inPos	    inPosition
#define inNormal	inNormal
#define inColor		inColor

#define outPos		gl_Position
#define outNormal   fragNormal
#define outColor   	fragColor
#define outUv		fragTexCoord
