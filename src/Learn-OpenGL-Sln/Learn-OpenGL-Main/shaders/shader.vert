#version 330 core

layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec3 attr_color;
  
out vec3 vertexColor; // > specify a color output to the fragment shader
out vec3 vertexPosition;

uniform float horizontalOffset;

void main()
{
    vertexPosition = vec3(attr_pos.x + horizontalOffset, attr_pos.y, attr_pos.z);
    gl_Position = vec4(vertexPosition, 1); 
    vertexColor = attr_color;
}