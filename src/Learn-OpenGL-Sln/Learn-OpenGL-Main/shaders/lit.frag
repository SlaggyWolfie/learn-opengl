#version 330 core

struct Material
{
	sampler2D diffuse, specular, emissive;
	float shininess;
};

struct TextureSamples { vec3 diffuse, specular, emissive; } samples;

struct LightAttenuation { float constant, linear, quadratic; };
struct LightColors { vec3 ambient, diffuse, specular; };

struct DirectionalLight
{
	vec3 direction;
	LightColors colors;
};

struct PointLight
{
	vec3 position;
	LightColors colors;
	LightAttenuation attenuation;
};

struct Spotlight
{
	vec3 position;
	vec3 direction;

	float innerCutoff;
	float outerCutoff;

	LightColors colors;
	LightAttenuation attenuation;
};

in vec2 textureCoordinate;

in vec3 fragmentPosition;
in vec3 fragmentNormal;

out vec4 fragmentColor;  

uniform Material material;
uniform vec3 viewerPosition;

uniform DirectionalLight directionalLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
//uniform Spotlight spotlight;

vec3 calculateDirectionalLight(DirectionalLight light, vec3 n_normal, vec3 n_viewDirection);
vec3 calculatePointLight(PointLight light, vec3 fragmentPosition, vec3 n_normal, vec3 n_viewDirection);
vec3 calculateSpotlight(Spotlight light, vec3 fragmentPosition, vec3 normal, vec3 viewDirection);

void main()
{
	vec3 result = vec3(0);

	// texture samples
	samples.diffuse = texture(material.diffuse, textureCoordinate).rgb;
	samples.specular = texture(material.specular, textureCoordinate).rgb;
	samples.emissive = texture(material.emissive, textureCoordinate).rgb;

	// normalize
	vec3 n_normal = normalize(fragmentNormal);
	vec3 n_viewDirection = normalize(viewerPosition - fragmentPosition);

	result += calculateDirectionalLight(directionalLight, n_normal, n_viewDirection);

	for (int i = 0; i < NR_POINT_LIGHTS; ++i)
		result += calculatePointLight(pointLights[i], fragmentPosition, n_normal, n_viewDirection);

//	result += calculateSpotlight(spotlight, fragmentPosition, n_normal, n_viewDirection);

	result += samples.emissive;

	fragmentColor = vec4(result, 1.0);
} 

vec3 calculateDirectionalLight(DirectionalLight light, vec3 n_normal, vec3 n_viewDirection)
{
	vec3 n_lightDirection = normalize(light.direction);

	float diffuseStrength = max(dot(n_normal, n_lightDirection), 0);

	vec3 reflectionDirection = reflect(-n_lightDirection, n_normal);
	float specularStrength = pow(max(dot(n_viewDirection, reflectionDirection), 0), material.shininess);

	vec3 ambientColor = light.colors.ambient * samples.diffuse;
	vec3 diffuseColor = light.colors.diffuse * diffuseStrength * samples.diffuse;
	vec3 specularColor = light.colors.specular * specularStrength * samples.specular;
	
	vec3 result = ambientColor + diffuseColor + specularColor;
	return result;
}

vec3 calculatePointLight(PointLight light, vec3 fragmentPosition, vec3 n_normal, vec3 n_viewDirection)
{
	vec3 lightDirection = light.position - fragmentPosition;
	vec3 n_lightDirection = normalize(lightDirection);

	float diffuseStrength = max(dot(n_normal, n_lightDirection), 0);

	vec3 reflectionDirection = reflect(-n_lightDirection, n_normal);
	float specularStrength = pow(max(dot(n_viewDirection, reflectionDirection), 0), material.shininess);
	
	// attenuation
	float lightDistance = length(lightDirection);
	LightAttenuation la = light.attenuation;
	// att = 1 / (constant + linear * distance + quadratic * distance^2)
	float attenuation = 1.0 / (la.constant + la.linear * lightDistance + la.quadratic * (lightDistance * lightDistance));

	vec3 ambientColor = light.colors.ambient * samples.diffuse;
	vec3 diffuseColor = light.colors.diffuse * diffuseStrength * samples.diffuse;
	vec3 specularColor = light.colors.specular * specularStrength * samples.specular;
	
	vec3 result = ambientColor + diffuseColor + specularColor;
	result *= attenuation;

	return result;
}

vec3 calculateSpotlight(Spotlight light, vec3 fragmentPosition, vec3 n_normal, vec3 n_viewDirection)
{
	vec3 result = vec3(0);

	vec3 ambientColor = light.colors.ambient * samples.diffuse;

	vec3 lightDirection = light.position - fragmentPosition;
	vec3 n_lightDirection = normalize(lightDirection);
	
	// attenuation
	float lightDistance = length(lightDirection);
	LightAttenuation la = light.attenuation;
	// att = 1 / (constant + linear * distance + quadratic * distance^2)
	float attenuation = 1.0 / (la.constant + la.linear * lightDistance + la.quadratic * (lightDistance * lightDistance));

	// cone calculations
	float theta = dot(n_lightDirection, normalize(-light.direction));
	float epsilon = light.innerCutoff - light.outerCutoff;
	//float intensity = clamp((theta - light.spotlightOuterCutOff) / epsilon, 0, 1); // Tutorial version
	float intensity = smoothstep(0, 1, (theta - light.outerCutoff) / epsilon);

	result = ambientColor * attenuation;

	if (theta > light.outerCutoff)
	{
		float diffuseStrength = max(dot(n_normal, n_lightDirection), 0);

		vec3 reflectionDirection = reflect(-n_lightDirection, n_normal);
		float specularStrength = pow(max(dot(n_viewDirection, reflectionDirection), 0), material.shininess);
		
		vec3 diffuseColor = light.colors.diffuse * diffuseStrength * samples.diffuse;
		vec3 specularColor = light.colors.specular * specularStrength * samples.specular;
		
		ambientColor *= attenuation;
		diffuseColor *= attenuation * intensity;
		specularColor *= attenuation * intensity;

		vec3 result = ambientColor + diffuseColor + specularColor;
	}

	return result;
}