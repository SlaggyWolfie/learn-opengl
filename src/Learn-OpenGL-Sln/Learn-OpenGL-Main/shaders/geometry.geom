#version 330 core
layout (points) in;
layout (points, max_vertices = 1) out;

// known as a pass-through shader
void main()
{
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	EndPrimitive();
}