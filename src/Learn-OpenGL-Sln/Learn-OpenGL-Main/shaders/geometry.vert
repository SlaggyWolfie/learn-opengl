#version 330 core

layout (location = 0) in vec2 attr_position;
layout (location = 1) in vec3 attr_color;

out VS_OUT
{
    vec3 color;
} vs_out;

void main()
{
    gl_Position = vec4(attr_position, 0, 1);
    vs_out.color = attr_color;
}