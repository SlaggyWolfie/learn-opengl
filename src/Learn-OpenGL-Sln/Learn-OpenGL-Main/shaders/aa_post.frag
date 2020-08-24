#version 330 core

out vec4 fragmentColor;  
in vec2 textureCoordinate;

uniform sampler2D screenTexture;

void main()
{
	vec3 color = texture(screenTexture, textureCoordinate).rgb;

	// magic (as usual)
	float grayscale = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
	fragmentColor = vec4(vec3(grayscale), 1);
} 