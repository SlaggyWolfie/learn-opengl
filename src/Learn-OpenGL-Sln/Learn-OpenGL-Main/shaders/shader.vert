#version 330 core

layout (location = 0) in vec3 attr_pos;
//layout (location = 1) in vec3 attr_color;
layout (location = 1) in vec2 attr_uv;  
  
//out vec3 vertexColor;
out vec2 textureCoordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    //gl_Position = vec4(attr_pos, 1);
    //vertexColor = attr_color;

    gl_Position = projection * view * model * vec4(attr_pos, 1);
    textureCoordinate = attr_uv;
    //textureCoordinate = vec2(attr_uv.x, 1 - attr_uv.y);
}