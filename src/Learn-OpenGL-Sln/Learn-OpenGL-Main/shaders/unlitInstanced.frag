#version 330 core

out vec4 fragmentColor;  
in vec2 textureCoordinate;

struct Material
{
	sampler2D texture_diffuse1, texture_diffuse2, texture_diffuse3;
	sampler2D texture_specular1, texture_specular2, texture_specular3;
	sampler2D texture_normal1, texture_normal2, texture_normal3;
	sampler2D texture_height1, texture_height2, texture_height3;
	sampler2D texture_emissive;
	float shininess;
};

uniform Material material;

void main()
{
	fragmentColor = texture(material.texture_diffuse1, textureCoordinate);
} 