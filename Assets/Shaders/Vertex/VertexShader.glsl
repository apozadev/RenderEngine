#version 450

#pragma shader_stage(vertex)

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUv;

layout(set = 0, binding = 0) uniform GlobalBuffer {
    mat4 viewproj;
};

layout(set = 2, binding = 0) uniform ModelBuffer {
    mat4 model;
};

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = viewproj * model * vec4(inPosition, 1.0);

    //gl_Position = viewproj * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inUv;
}