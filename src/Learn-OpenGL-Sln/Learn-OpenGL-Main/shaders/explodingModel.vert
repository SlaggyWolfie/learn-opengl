#version 330 core

layout (location = 0) in vec3 attr_position;
layout (location = 1) in vec3 attr_normal;
layout (location = 2) in vec2 attr_uv;

out VS_OUT
{
    vec2 uv;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(attr_position, 1);    
    vs_out.uv = attr_uv;
}