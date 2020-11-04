#version 330 core

#define MAX_LIGHTS 4

out vec4 fragmentColor;  

in VS_OUT
{
	vec3 fragmentPosition;
	vec3 normal;
	vec2 textureCoordinate;
} fs_in;

uniform sampler2D textureSampler;

uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform vec3 viewPosition;
uniform bool gamma;

vec3 BlingPhong(vec3 n_normal, vec3 fragmentPosition, vec3 lightPosition, vec3 lightColor);

void main()
{
	vec3 color = texture(textureSampler, fs_in.textureCoordinate).rgb;
	vec3 n_normal = normalize(fs_in.normal);

	vec3 ambient = 0.05 * color;
	vec3 lighting = ambient;
	// vec3 lighting = vec3(0);

	for (int i = 0; i < MAX_LIGHTS; ++i)
		lighting += BlingPhong(n_normal, fs_in.fragmentPosition, lightPositions[i], lightColors[i]);

	color *= lighting;

	if (gamma) color = pow(color, vec3(1.0/2.2));

	fragmentColor = vec4(color, 1);
} 

vec3 BlingPhong(vec3 n_normal, vec3 fragmentPosition, vec3 lightPosition, vec3 lightColor)
{
	vec3 n_lightDirection = normalize(lightPosition - fragmentPosition);
	vec3 n_viewDirection = normalize(viewPosition - fragmentPosition);
	vec3 n_halfwayDirection = normalize(n_lightDirection + n_viewDirection);

	float diff = max(dot(n_lightDirection, n_normal), 0.0);
	float spec = 0; 
	if (abs(diff) > 0.00001)
		spec = pow(max(dot(n_normal, n_halfwayDirection), 0.0), 64.0);

	// > simple attenuation
	// float maxDistance = 1.5;
	float dist = length(lightPosition - fragmentPosition);
	float attenuation = 1 / (gamma? dist * dist : dist);

	vec3 diffuse = (diff * lightColor) * attenuation;
	vec3 specular = (spec * lightColor) * attenuation; 

	return diffuse + specular;
}