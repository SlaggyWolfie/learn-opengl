#version 330 core

layout (location = 0) in vec3 attr_pos; 
  
out vec3 cubemapDirection;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 position = projection * view * vec4(attr_pos, 1);
    cubemapDirection = attr_pos;

    gl_Position = position.xyww;
}