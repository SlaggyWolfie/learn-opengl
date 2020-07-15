#version 330 core

layout (location = 0) in vec2 attr_pos;
layout (location = 1) in vec3 attr_color;

out vec3 vertexColor;

uniform vec2 offsets[100];

void main()
{
	vec2 offset = offsets[gl_InstanceID];
	gl_Position = vec4(attr_pos + offset, 0, 1);
	vertexColor = attr_color;
}