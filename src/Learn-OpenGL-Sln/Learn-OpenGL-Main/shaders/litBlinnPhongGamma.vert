#version 330 core

layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec3 attr_normal;  
layout (location = 2) in vec2 attr_uv;
  
out VS_OUT 
{
    vec3 fragmentPosition;
    vec3 normal;
    vec2 textureCoordinate;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 position = vec3(model * vec4(attr_pos, 1));

    vs_out.fragmentPosition = position;
    vs_out.normal = attr_normal;
    vs_out.textureCoordinate = attr_uv;

    gl_Position = projection * view * vec4(position, 1);    
}