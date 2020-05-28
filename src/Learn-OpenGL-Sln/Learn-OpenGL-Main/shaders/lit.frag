#version 330 core

in vec2 textureCoordinate;

in vec3 fragmentPosition;
in vec3 fragmentNormal;

out vec4 fragmentColor;  

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
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
	// texture samples
	vec3 diffuseSample = vec3(texture(material.diffuse, textureCoordinate));
	vec3 specularSample = vec3(texture(material.specular, textureCoordinate));
	specularSample = vec3(1) - specularSample;

	// ambient
	// first parameter is intensity
	vec3 ambientColor = light.ambientColor * diffuseSample;
	
	// diffuse
	vec3 nNormal = normalize(fragmentNormal);
	vec3 nLightDirection = normalize(light.position - fragmentPosition);

	float diffuseStrength = max(dot(nNormal, nLightDirection), 0.0);
	vec3 diffuseColor = light.diffuseColor * (diffuseStrength * diffuseSample);

	// specular
	vec3 nViewerDirection = normalize(viewerPosition - fragmentPosition);
	vec3 reflectionDirection = normalize(reflect(-nLightDirection, nNormal));

	float specularStrength = pow(max(dot(nViewerDirection, reflectionDirection), 0.0), material.shininess);
	vec3 specularColor = light.specularColor * (specularSample * specularStrength);

	vec3 result = ambientColor + diffuseColor + specularColor;

	fragmentColor = vec4(result, 1.0);
} 