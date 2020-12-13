#version 330 core

layout (location = 0) in vec3 attr_pos;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(attr_pos, 1);
}