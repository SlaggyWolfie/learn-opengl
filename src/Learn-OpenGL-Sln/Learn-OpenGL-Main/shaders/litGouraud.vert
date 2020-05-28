#version 330 core

layout (location = 0) in vec3 attr_position;
layout (location = 1) in vec3 attr_normal;  

out vec3 gouraudColor;

uniform vec3 viewerPosition;
uniform vec3 lightPosition;
uniform vec3 lightColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{    
    vec3 worldPosition = vec3(model * vec4(attr_position, 1.0));
    gl_Position = projection * view * vec4(worldPosition, 1);

    vec3 worldNormal = mat3(transpose(inverse(model))) * attr_normal;

    // ambient
	float ambientStrength = 0.1;
	vec3 ambientColor = ambientStrength * lightColor;
	
	// diffuse
	vec3 nNormal = normalize(worldNormal);
	vec3 nLightDirection = normalize(lightPosition - worldPosition);

	float diffuseStrength = max(dot(nNormal, nLightDirection), 0.0);
	vec3 diffuseColor = diffuseStrength * lightColor;

	// specular
	float specularStrength = 0.5;
	int shininess = 256;

	vec3 nViewerDirection = normalize(viewerPosition - worldPosition);
	vec3 reflectionDirection = normalize(reflect(-nLightDirection, nNormal));

	float specular = pow(max(dot(nViewerDirection, reflectionDirection), 0.0), shininess);
	vec3 specularColor = specularStrength * specular * lightColor;

	gouraudColor = ambientColor + diffuseColor + specularColor;
}