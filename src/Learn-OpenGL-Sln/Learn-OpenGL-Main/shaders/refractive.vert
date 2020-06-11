#version 330 core

layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec3 attr_normal;  
layout (location = 2) in vec2 attr_uv;  
  
out vec3 fragmentPosition;
out vec3 fragmentNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    fragmentPosition = vec3(model * vec4(attr_pos, 1));
    fragmentNormal = mat3(inverse(transpose(model))) * attr_normal;
    gl_Position = projection * view * vec4(fragmentPosition, 1);
}