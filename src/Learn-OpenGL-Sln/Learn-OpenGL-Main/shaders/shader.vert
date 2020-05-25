#version 330 core

layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec3 attr_color;
layout (location = 2) in vec2 attr_uv;  
  
//out vec3 vertexColor;
out vec2 textureCoordinate;

void main()
{
    gl_Position = vec4(attr_pos, 1);
//    vertexColor = attr_color;
    textureCoordinate = attr_uv;
}