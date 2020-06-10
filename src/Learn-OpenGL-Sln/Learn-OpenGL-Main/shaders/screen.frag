#version 330 core

out vec4 fragmentColor;  
in vec2 textureCoordinate;
uniform sampler2D textureSampler;

const float offset = 1 / 300.0;

const vec2 offsets[9] = vec2[]
(
    vec2(-offset,  offset), // top-left
    vec2( 0.0f,    offset), // top-center
    vec2( offset,  offset), // top-right
    vec2(-offset,  0.0f),   // center-left
    vec2( 0.0f,    0.0f),   // center-center
    vec2( offset,  0.0f),   // center-right
    vec2(-offset, -offset), // bottom-left
    vec2( 0.0f,   -offset), // bottom-center
    vec2( offset, -offset)  // bottom-right    
);

const float sharpenKernel[9] = float[]
(
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1
);

const float blurKernel[9] = float[]
(
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

// edge detection kernel
const float edgeKernel[9] = float[]
(
    1,  1, 1,
    1, -8, 1,
    1,  1, 1
);

void main()
{
	// standard
	vec4 sampled = texture(textureSampler, textureCoordinate);
	fragmentColor = sampled;

	// inversion
    //fragmentColor = vec4(vec3(1 - sampled), 1);

	// grayscale
    //// bad
    //float average = (sampled.r + sampled.g + sampled.b) / 3;
    //fragmentColor = vec4(vec3(average), 1);

    //// good
    //float average = 0.2126 * sampled.r + 0.7152 * sampled.g + 0.0722 * sampled.b;
    //fragmentColor = vec4(vec3(average), 1);

    float[9] kernel;
    //kernel = sharpenKernel;
    //kernel = blurKernel;
    kernel = edgeKernel;

    // kerneling
    //vec3 sampleAround[9];
    vec3 color = vec3(0);
    for (int i = 0; i < 9; ++i)
    {
        //sampleAround[i] = vec3(texture(textureSampler, textureCoordinate + offsets[i]));
        vec3 sampleAround = vec3(texture(textureSampler, textureCoordinate + offsets[i]));
        color += sampleAround * kernel[i];
    }

    fragmentColor = vec4(color, 1);
} 