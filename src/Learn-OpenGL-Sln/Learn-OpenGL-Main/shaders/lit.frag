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
	vec3 ambientColor = lightColor * ambientStrength;
	
	// diffuse
	vec3 nNormal = normalize(fragmentNormal);
	vec3 nLightDirection = normalize(lightPosition - fragmentPosition);

	float diffuseStrength = max(dot(nLightDirection, nNormal), 0.0);
	vec3 diffuseColor = lightColor * diffuseStrength;

	// specular
	float specularStrength = 0.5;
	int shininess = 256;

	vec3 nViewerDirection = normalize(viewerPosition - lightPosition);
	vec3 reflectionDirection = reflect(-nLightDirection, nNormal);

	float specular = pow(max(dot(nViewerDirection, reflectionDirection), 0.0), shininess);
	vec3 specularColor = lightColor * specular * specularStrength;

	vec3 result = (ambientColor + diffuseColor + specularColor) * objectColor;

	fragmentColor = vec4(result, 1.0);
} 