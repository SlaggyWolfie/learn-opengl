#version 330 core

in vec3 fragmentPosition;
in vec3 fragmentNormal;

out vec4 fragmentColor;  

struct Material
{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	float shininess;
};

struct Light
{
	vec3 position;

	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
};

uniform Material material;
uniform Light light;

uniform vec3 viewerPosition;

void main()
{
	// ambient
	// first parameter is intensity
	vec3 ambientColor = vec3(0.1) * light.ambientColor * material.ambientColor;
	
	// diffuse
	vec3 nNormal = normalize(fragmentNormal);
	vec3 nLightDirection = normalize(light.position - fragmentPosition);

	float diffuseStrength = max(dot(nNormal, nLightDirection), 0.0);
	vec3 diffuseColor = light.diffuseColor * (diffuseStrength * material.diffuseColor);

	// specular
	float specularStrength = 0.5;
	int shininess = 256;

	vec3 nViewerDirection = normalize(viewerPosition - fragmentPosition);
	vec3 reflectionDirection = normalize(reflect(-nLightDirection, nNormal));

	float specular = pow(max(dot(nViewerDirection, reflectionDirection), 0.0), material.shininess);
	vec3 specularColor = light.specularColor * (material.specularColor * specular);

	vec3 result = ambientColor + diffuseColor + specularColor;

	fragmentColor = vec4(result, 1.0);
} 