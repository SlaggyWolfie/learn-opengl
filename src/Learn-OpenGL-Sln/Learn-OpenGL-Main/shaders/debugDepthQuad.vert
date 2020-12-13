#version 330 core

layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec2 attr_uv;
  
out vec2 textureCoordinate;

void main()
{
    gl_Position = vec4(attr_pos, 1);
    textureCoordinate = attr_uv;
}