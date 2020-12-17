#version 330 core

layout (location = 0) in vec3 attr_pos;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(attr_pos, 1);
}