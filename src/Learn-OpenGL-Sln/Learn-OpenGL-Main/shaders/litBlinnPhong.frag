#version 330 core

out vec4 fragmentColor;  


in VS_OUT
{
	vec3 fragmentPosition;
	vec3 normal;
	vec2 textureCoordinate;
} fs_in;

uniform sampler2D textureSampler;

uniform vec3 lightPosition;
uniform vec3 viewPosition;
uniform bool isBlinn;

void main()
{
	vec3 color = texture(textureSampler, fs_in.textureCoordinate).rgb;

	vec3 ambient = 0.05 * color;

	vec3 n_lightDirection = normalize(lightPosition - fs_in.fragmentPosition);
	vec3 n_normal = normalize(fs_in.normal);

	float diff = max(dot(n_lightDirection, n_normal), 0.0);
	vec3 diffuse = diff * color;

	vec3 n_viewDirection = normalize(viewPosition - fs_in.fragmentPosition);

	float spec = 0;
	if (isBlinn)
	{
		vec3 n_halfwayDirection = normalize(n_lightDirection + n_viewDirection);
		spec = pow(max(dot(n_normal, n_halfwayDirection), 0.0), 32.0);
	}
	else
	{
		vec3 reflectionDirection = reflect(-n_lightDirection, n_normal);
		spec = pow(max(dot(n_viewDirection, reflectionDirection), 0.0), 8.0);
	}

	// assumes bright white color
	vec3 specular = vec3(0.3) * spec; 

	fragmentColor = vec4(ambient + diffuse + specular, 1);
} 