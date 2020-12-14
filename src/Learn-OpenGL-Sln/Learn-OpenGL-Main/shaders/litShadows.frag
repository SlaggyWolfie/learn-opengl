#version 330 core

out vec4 fragmentColor;  


in VS_OUT
{
	vec3 fragmentPosition;
	vec3 normal;
	vec2 textureCoordinate;
	vec4 fragmentPositionLightSpace;
} fs_in;

uniform sampler2D textureSampler;
uniform sampler2D shadowMap;

uniform vec3 lightPosition;
uniform vec3 viewPosition;

float ShadowCalc(vec4 fragmentPositionLightSpace, vec3 normal, vec3 lightDirection);

void main()
{
	vec3 color = texture(textureSampler, fs_in.textureCoordinate).rgb;
	vec3 lightColor = vec3(0.3);

	vec3 ambient = 0.15 * color;

	vec3 n_lightDirection = normalize(lightPosition - fs_in.fragmentPosition);
	vec3 n_normal = normalize(fs_in.normal);
	
	float diff = max(dot(n_lightDirection, n_normal), 0.0);
	vec3 diffuse = diff * color;

	vec3 n_viewDirection = normalize(viewPosition - fs_in.fragmentPosition);

	vec3 reflectionDirection = reflect(-n_lightDirection, n_normal);
	float spec = pow(max(dot(n_viewDirection, reflectionDirection), 0.0), 64.0);
	vec3 specular = lightColor * spec; 
	
	float shadowed = ShadowCalc(fs_in.fragmentPositionLightSpace, n_normal, n_lightDirection);
	vec3 lighting = ambient  + (1 - shadowed) * (diffuse + specular);

	fragmentColor = vec4(lighting, 1);
} 

float ShadowCalc(vec4 fragmentPositionLightSpace, vec3 normal, vec3 lightDirection)
{
	// > perform perspective divide (for NDC)
	// [-1; 1]
	vec3 projectedCoords = fragmentPositionLightSpace.xyz / fragmentPositionLightSpace.w;
	// [0; 1]
	projectedCoords = projectedCoords * 0.5 + 0.5;
	float currentDepth = projectedCoords.z;
	if (currentDepth > 1) return 0;

	float minBias = 0.005, maxBias = 0.05;
	float bias = max(maxBias * (1 - dot(normal, lightDirection)), minBias);

	float closestDepth = texture(shadowMap, projectedCoords.xy).r;
	float shadow = currentDepth - bias > closestDepth ? 1 : 0;

	return shadow;
}