#version 330 core

out vec4 fragmentColor;  

in vec3 fragmentNormal;
in vec3 fragmentPosition;

uniform vec3 cameraPosition;
uniform samplerCube cubemap;

void main()
{
	vec3 n_incoming = normalize(fragmentPosition - cameraPosition);
	vec3 reflected = reflect(n_incoming, normalize(fragmentNormal));
	fragmentColor = vec4(texture(cubemap, reflected).rgb, 1);
} 