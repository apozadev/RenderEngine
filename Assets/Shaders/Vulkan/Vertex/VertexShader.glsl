#version 450

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

void main() {

    gl_Position = viewproj * PushConstants.model * vec4(inPosition, 1.0);   

    //gl_Position.y = -gl_Position.y;
    //gl_Position.x = -gl_Position.x;

    //gl_Position = viewproj * vec4(inPosition, 1.0);

    fragNormal = normalize(vec3(PushConstants.normal * vec4(inNormal, 1.0)));

    fragColor = inColor;
    fragTexCoord = inUv;
}