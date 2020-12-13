#version 330 core

out vec4 fragmentColor;  
in vec2 textureCoordinate;
uniform sampler2D textureSampler;

uniform float nearPlane;
uniform float farPlane;

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));	
}


void main()
{
	float depth = texture(textureSampler, textureCoordinate).r;
	fragmentColor = vec4(vec3(LinearizeDepth(depth) / farPlane), 1);
//	fragmentColor = vec4(vec3(depth), 1); // ortographic
} 