#version 450

#pragma shader_stage(fragment)

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(set = 2, binding = 0) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {

    vec3 lightDir = vec3(1, 0, 0);

    //outColor = vec4(fragColor, 1.0);
    //outColor = texture(texSampler, fragTexCoord);    
    //outColor = vec4(1,0,0,1);  
    //outColor = vec4(normalize(fragNormal), 1.0);
        
    vec4 color = texture(texSampler, fragTexCoord);

    float ambientFactor = 0.1;

    outColor = color * (max(0, dot(lightDir, fragNormal)) + ambientFactor);
    
}