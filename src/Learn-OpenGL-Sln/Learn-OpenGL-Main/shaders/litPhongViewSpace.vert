#version 330 core

layout (location = 0) in vec3 attr_position;
layout (location = 1) in vec3 attr_normal;  

out vec3 fragmentPosition;
out vec3 fragmentNormal;
out vec3 fragmentLightPosition;

uniform vec3 lightPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(attr_position, 1);
    
    fragmentPosition = vec3(view * model * vec4(attr_position, 1.0));
    fragmentNormal = mat3(transpose(inverse(view * model))) * attr_normal;
    fragmentLightPosition = vec3(view * vec4(lightPosition, 1));
}