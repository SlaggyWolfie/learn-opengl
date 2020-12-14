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

float ShadowCalc(vec4 fragmentPositionLightSpace);

void main()
{
	vec3 color = texture(textureSampler, fs_in.textureCoordinate).rgb;
	vec3 lightColor = vec3(1);

	vec3 ambient = 0.15 * color;

	vec3 n_lightDirection = normalize(lightPosition - fs_in.fragmentPosition);
	vec3 n_normal = normalize(fs_in.normal);
	
	float diff = max(dot(n_lightDirection, n_normal), 0.0);
	vec3 diffuse = diff * color;

	vec3 n_viewDirection = normalize(viewPosition - fs_in.fragmentPosition);

	vec3 reflectionDirection = reflect(-n_lightDirection, n_normal);
	float spec = pow(max(dot(n_viewDirection, reflectionDirection), 0.0), 8.0);
	vec3 specular = lightColor * spec; 
	
	vec3 lighting = ambient + diffuse + specular;

	float shadowed = ShadowCalc(fs_in.fragmentPositionLightSpace);
	fragmentColor = vec4(lighting + (1 - shadowed) * (diffuse + specular), 1);
} 

float ShadowCalc(vec4 fragmentPositionLightSpace)
{
	// > perform perspective divide (for NDC)
	// [-1; 1]
	vec3 projectedCoords = fragmentPositionLightSpace.xyz / fragmentPositionLightSpace.w;
	// [0; 1]
	projectedCoords = projectedCoords * 0.5 + 0.5;

	float closestDepth = texture(shadowMap, projectedCoords.xy).r;
	float currentDepth = projectedCoords.z;
	float shadow = currentDepth > closestDepth ? 1 : 0;

	return shadow;
}