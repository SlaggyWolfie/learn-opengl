#version 330 core

out vec4 fragColor;  

in vec3 vertexColor; // lerped
in vec2 textureCoordinate;

uniform sampler2D textureSampler;

void main()
{
	fragColor = texture(textureSampler, textureCoordinate) * vec4(vertexColor, 1);
} 