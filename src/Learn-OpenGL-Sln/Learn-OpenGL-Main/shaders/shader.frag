#version 330 core

out vec4 fragColor;  

in vec3 vertexColor; // lerped
in vec2 textureCoordinate;

uniform sampler2D textureSampler1;
uniform sampler2D textureSampler2;

void main()
{
	fragColor = mix(
		texture(textureSampler1, textureCoordinate), 
		texture(textureSampler2, textureCoordinate), 
		0.2f);
} 