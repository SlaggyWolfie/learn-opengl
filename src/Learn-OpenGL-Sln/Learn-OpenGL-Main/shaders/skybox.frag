#version 330 core

out vec4 fragmentColor;  
in vec3 cubemapDirection;
uniform samplerCube cubemap;

void main()
{
	fragmentColor = texture(cubemap, cubemapDirection);
} 