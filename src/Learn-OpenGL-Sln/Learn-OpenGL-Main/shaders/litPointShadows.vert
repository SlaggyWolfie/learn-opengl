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

uniform bool reverseNormals;

void main()
{
    vec3 position = vec3(model * vec4(attr_pos, 1));

    vs_out.fragmentPosition = position;
    vs_out.textureCoordinate = attr_uv;

    // > a slight hack to make sure the outer large cube displays lighting from the 'inside' instead of the default 'outside'.
    if (reverseNormals) 
        vs_out.normal = transpose(inverse(mat3(model))) * (-1 * attr_normal);
    else
        vs_out.normal = transpose(inverse(mat3(model))) * attr_normal;
    
    //vs_out.normal = attr_normal;

    gl_Position = projection * view * vec4(position, 1);    
}