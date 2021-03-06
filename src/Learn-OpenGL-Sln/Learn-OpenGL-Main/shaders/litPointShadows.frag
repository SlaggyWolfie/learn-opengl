#version 330 core

out vec4 fragmentColor;

in VS_OUT
{
	vec3 fragmentPosition;
	vec3 normal;
	vec2 textureCoordinate;
} fs_in;

uniform sampler2D textureSampler;
uniform samplerCube cubeShadowMap;

uniform vec3 lightPosition;
uniform vec3 viewPosition;

uniform float farPlane;

uniform bool shadowsOn;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
); 

float ShadowCalc(vec3 fragmentPosition);

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
	
	float shadowed = shadowsOn ? ShadowCalc(fs_in.fragmentPosition) : 0;
	vec3 lighting = ambient  + (1 - shadowed) * (diffuse + specular);

	fragmentColor = vec4(lighting, 1);
} 

float ShadowCalc(vec3 fragmentPosition)
{
    // > get vector between fragment position and light position
	vec3 fragmentToLight = fragmentPosition - lightPosition;

	// > now get current linear depth as the length between the fragment and light position
	float currentDepth = length(fragmentToLight);
	
	// > we use a much larger bias since depth is now in [near_plane, far_plane] range
	float bias = 0.05;
	float shadow = 0;
	int samples = 20;

	float viewDistance = length(viewPosition - fragmentPosition);
	float diskRadius = (1 + (viewDistance / farPlane)) / 25.0;

	for (int i = 0; i < samples; ++i)
	{
		// > use the fragment to light vector + surrounding area to sample from the depth map  
		float closestDepth = texture(cubeShadowMap, fragmentToLight + sampleOffsetDirections[i] * diskRadius).r;

		// > it is currently in linear range between [0,1], let's re-transform it back to original depth value
		closestDepth *= farPlane;

		if (currentDepth - bias > closestDepth) shadow++;
	}
	
	shadow /= float(samples); // shadow /= 20

	return shadow;

//	// > perform perspective divide (for NDC)
//	// [-1; 1]
//	vec3 projectedCoords = fragmentPositionLightSpace.xyz / fragmentPositionLightSpace.w;
//	// [0; 1]
//	projectedCoords = projectedCoords * 0.5 + 0.5;
//	float currentDepth = projectedCoords.z;
//	if (currentDepth > 1) return 0;
//
//	float minBias = 0.005, maxBias = 0.05;
//	float bias = max(maxBias * (1 - dot(normal, lightDirection)), minBias);
//
////	float closestDepth = texture(shadowMap, projectedCoords.xy).r;
////	float shadow = currentDepth - bias > closestDepth ? 1 : 0;
//
//	float shadow = 0;
//	vec2 texelSize = 1 / textureSize(shadowMap, 0);
//
//	// 3x3 sampling around
//	// PCF
//	for (int x = -1; x <= 1; ++x)
//	{
//		for (int y = -1; y <= 1; ++y)
//		{
//			float depth = texture(shadowMap, projectedCoords.xy + vec2(x, y) * texelSize).r;
//			shadow += currentDepth - bias > depth ? 1 : 0;
//		}
//	}
//	
//	shadow /= 9;
//
//	return shadow;
}