#version 330 core

out vec4 fragColor;  
in vec3 vertexColor; // lerped
in vec3 vertexPosition; // lerped

void main()
{
    //fragColor = vec4(vertexColor, 1);
    fragColor = vec4(vertexPosition, 1);
} 

// Answer to why the lower left angle is black:
// Because of interpolation and the negative values due to do NDC
// aka the coordinates are negative (-0.5) so they get clamped to 0.0f
// and 0 is black, so that's why it happens.