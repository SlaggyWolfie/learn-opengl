#version 330 core

layout (location = 0) in vec2 attr_pos;
layout (location = 1) in vec3 attr_color;
layout (location = 2) in vec2 attr_offset;

out vec3 vertexColor;

uniform vec2 offsets[100];

void main()
{
	vec2 position = attr_pos * (gl_InstanceID / 100.0f);
	gl_Position = vec4(position + attr_offset, 0, 1);
	vertexColor = attr_color;
}