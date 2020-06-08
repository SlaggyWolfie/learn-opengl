#version 330 core

out vec4 fragmentColor;  
in vec2 textureCoordinate;
uniform sampler2D textureSampler;

float near = 0.1;
float far = 100;

float linearizeDepth(float depth)
{
	float z = depth * 2 - 1; // > back to NDC
	return (2 * near * far) / (far + near - z * (far - near));
}

void main()
{
	// > divide by far for demo purposes
	float depth = linearizeDepth(gl_FragCoord.z) / far; 
	fragmentColor = vec4(vec3(depth), 1);
} 