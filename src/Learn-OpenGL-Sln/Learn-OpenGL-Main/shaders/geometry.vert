#version 330 core

layout (location = 0) in vec2 attr_pos;

void main()
{
    gl_Position = vec4(attr_pos, 0, 1);
}