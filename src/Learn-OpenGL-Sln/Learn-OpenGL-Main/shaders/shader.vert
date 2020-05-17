#version 330 core

layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec3 attr_color;
  
out vec3 vertexColor; // > specify a color output to the fragment shader

void main()
{
    gl_Position = vec4(attr_pos, 1); 
    vertexColor = attr_color;
}