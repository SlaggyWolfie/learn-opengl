#version 330 core

layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec3 attr_normal;
layout (location = 2) in vec2 attr_uv;  
layout (location = 3) in mat4 attr_instance_modelMatrix;
  
out vec2 textureCoordinate;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * attr_instance_modelMatrix * vec4(attr_pos, 1);
    textureCoordinate = attr_uv;
}