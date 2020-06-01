#version 330 core

in vec2 textureCoordinate;

in vec3 fragmentPosition;
in vec3 fragmentNormal;

out vec4 fragmentColor;  

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
//	sampler2D emissive;
	float shininess;
};

struct LightAttenuation
{
	float constant;
	float linear;
	float quadratic;
};

struct Light
{
	vec3 position;
	vec3 spotlightDirection;

	float spotlightCutOff;

	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;

	LightAttenuation attenuation;
};

uniform Material material;
uniform Light light;

uniform vec3 viewerPosition;

void main()
{
	vec3 result = vec3(0);

	// texture samples
	vec3 diffuseSample = texture(material.diffuse, textureCoordinate).rgb;
	vec3 specularSample = texture(material.specular, textureCoordinate).rgb;
//	vec3 emissiveSample = texture(material.emissive, textureCoordinate).rgb;

	// ambient
	vec3 ambientColor = light.ambientColor * diffuseSample;
	
	// diffuse
	vec3 nNormal = normalize(fragmentNormal);
//	vec3 nLightDirection = normalize(-light.direction);
	vec3 lightDirection = light.position - fragmentPosition;
	vec3 nLightDirection = normalize(lightDirection);

	float theta = dot(nLightDirection, normalize(-light.spotlightDirection));

	if (theta > light.spotlightCutOff)
	{
		float diffuseStrength = max(dot(nNormal, nLightDirection), 0.0);
		vec3 diffuseColor = light.diffuseColor * (diffuseStrength * diffuseSample);

		// specular
		vec3 nViewerDirection = normalize(viewerPosition - fragmentPosition);
		vec3 reflectionDirection = normalize(reflect(-nLightDirection, nNormal));

		float specularStrength = pow(max(dot(nViewerDirection, reflectionDirection), 0.0), material.shininess);
		vec3 specularColor = light.specularColor * (specularSample * specularStrength);

		// attenuation calculations
		float lightDistance = length(lightDirection);
		LightAttenuation la = light.attenuation;
		// att = 1 / (constant + linear * distance + quadratic * distance^2)
		float attenuation = 1.0 / (la.constant + la.linear * lightDistance + la.quadratic * (lightDistance * lightDistance));

//		ambientColor *= attenuation;
		diffuseColor *= attenuation;
		specularColor *= attenuation;

		result = ambientColor + diffuseColor + specularColor;
	}
	else
	{
		result = ambientColor;
	}

//	result += emissiveSample;

	fragmentColor = vec4(result, 1.0);
} 