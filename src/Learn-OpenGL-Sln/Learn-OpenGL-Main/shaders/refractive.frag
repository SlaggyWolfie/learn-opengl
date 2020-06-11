#version 330 core

out vec4 fragmentColor;  

in vec3 fragmentNormal;
in vec3 fragmentPosition;

uniform vec3 cameraPosition;
uniform samplerCube cubemap;

void main()
{
	float ratio = 1 / 1.52;
	vec3 n_incoming = normalize(fragmentPosition - cameraPosition);
	vec3 reflected = refract(n_incoming, normalize(fragmentNormal), ratio);
	fragmentColor = vec4(texture(cubemap, reflected).rgb, 1);
} 