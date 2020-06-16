#version 330 core

layout (location = 0) in vec3 attr_position;
layout (location = 1) in vec3 attr_normal;
//layout (location = 2) in vec2 attr_uv;

out VS_OUT
{
    vec3 normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;

void main()
{
    gl_Position = view * model * vec4(attr_position, 1);
    mat3 normalMatrix = mat3(inverse(transpose(view * model)));
    vs_out.normal = normalMatrix * attr_normal;
}