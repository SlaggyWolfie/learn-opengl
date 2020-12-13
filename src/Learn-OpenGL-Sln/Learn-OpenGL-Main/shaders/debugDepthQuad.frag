#version 330 core

out vec4 fragmentColor;  
in vec2 textureCoordinate;
uniform sampler2D textureSampler;

void main()
{
	float depth = texture(textureSampler, textureCoordinate).r;
	fragmentColor = vec4(vec3(depth), 1);
} 