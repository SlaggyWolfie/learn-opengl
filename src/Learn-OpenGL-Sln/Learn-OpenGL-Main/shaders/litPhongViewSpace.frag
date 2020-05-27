#version 330 core

in vec3 fragmentPosition;
in vec3 fragmentNormal;
in vec3 fragmentLightPosition;

out vec4 fragmentColor;  

uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
	// ambient
	float ambientStrength = 0.1;
	vec3 ambientColor = ambientStrength * lightColor;
	
	// diffuse
	vec3 nNormal = normalize(fragmentNormal);
	vec3 nLightDirection = normalize(fragmentLightPosition - fragmentPosition);

	float diffuseStrength = max(dot(nNormal, nLightDirection), 0.0);
	vec3 diffuseColor =  diffuseStrength * lightColor;

	// specular
	float specularStrength = 0.5;
	int shininess = 256;

	vec3 nViewerDirection = normalize(-fragmentPosition);
	vec3 reflectionDirection = reflect(-nLightDirection, nNormal);

	float specular = pow(max(dot(nViewerDirection, reflectionDirection), 0.0), shininess);
	vec3 specularColor = specular * specularStrength * lightColor;

	vec3 result = (ambientColor + diffuseColor + specularColor) * objectColor;

	fragmentColor = vec4(result, 1.0);
} 