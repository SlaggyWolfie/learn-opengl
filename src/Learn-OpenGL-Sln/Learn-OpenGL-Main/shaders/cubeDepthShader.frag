#version 330 core

in vec4 fragmentPosition;

uniform vec3 lightPosition;
uniform float farPlane;

void main()
{
	// > get distance between fragment and light source
	float lightDistance = length(fragmentPosition.xyz - lightPosition);

	// > map to [0;1] rangne by dividing by farPlane
	lightDistance /= farPlane;

	// > write this as modified depth
	gl_FragDepth = lightDistance;
} 