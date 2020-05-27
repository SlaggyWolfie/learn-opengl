#version 330 core

in vec3 fragmentPosition;
in vec3 fragmentNormal;

out vec4 fragmentColor;  

uniform vec3 viewerPosition;

uniform vec3 lightPosition;
uniform vec3 lightColor;

uniform vec3 objectColor;

void main()
{
	// ambient
	float ambientStrength = 0.1;
	vec3 ambientColor = ambientStrength * lightColor;
	
	// diffuse
	vec3 nNormal = normalize(fragmentNormal);
	vec3 nLightDirection = normalize(lightPosition - fragmentPosition);

	float diffuseStrength = max(dot(nNormal, nLightDirection), 0.0);
	vec3 diffuseColor = diffuseStrength * lightColor;

	// specular
	float specularStrength = 0.5;
	int shininess = 256;

	vec3 nViewerDirection = normalize(viewerPosition - fragmentPosition);
	vec3 reflectionDirection = normalize(reflect(-nLightDirection, nNormal));

	float specular = pow(max(dot(nViewerDirection, reflectionDirection), 0.0), shininess);
	vec3 specularColor = specularStrength * specular * lightColor;

	vec3 result = (ambientColor + diffuseColor + specularColor) * objectColor;

	fragmentColor = vec4(result, 1.0);
} 