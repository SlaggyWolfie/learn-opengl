#version 330 core

out vec4 fragmentColor;  
in vec2 textureCoordinate;
uniform sampler2D textureSampler;

void main()
{
	// standard
	vec4 sampled = texture(textureSampler, textureCoordinate);
	fragmentColor = sampled;

	// inversion
//	fragmentColor = vec4(vec3(1 - sampled), 1);

	// grayscale
//	// bad
//	float average = (sampled.r + sampled.g + sampled.b) / 3;
//	fragmentColor = vec4(vec3(average), 1);

	// good
	float average = 0.2126 * sampled.r + 0.7152 * sampled.g + 0.0722 * sampled.b;
	fragmentColor = vec4(vec3(average), 1);
} 