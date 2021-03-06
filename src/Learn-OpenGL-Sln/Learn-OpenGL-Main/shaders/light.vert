#version 330 core

layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec2 attr_uv;  

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(attr_pos, 1);
}