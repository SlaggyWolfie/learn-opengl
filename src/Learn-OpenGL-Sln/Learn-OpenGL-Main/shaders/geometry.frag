#version 330 core

in vec3 geometryColor;
out vec4 fragmentColor;

void main()
{
	fragmentColor = vec4(geometryColor, 1);
} 