#version 330 core

layout (location = 0) in vec3 attr_position;
layout (location = 1) in vec3 attr_normal;  

out vec3 fragmentPosition;
out vec3 fragmentNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 mvp;
uniform mat3 normalMatrix;

void main()
{
    gl_Position = mvp * vec4(attr_position, 1);
    
    fragmentPosition = vec3(model * vec4(attr_position, 1.0));
//    fragmentNormal = attr_normal;
//    fragmentNormal = mat3(transpose(inverse(model))) * attr_normal;
    fragmentNormal = normalMatrix * attr_normal;
}