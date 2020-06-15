#include "programs/GeometryShaderProgram.hpp"
#include "programs/UniformBufferProgram.hpp"

int main(int argc, char* argv[])
{
	Program* program = nullptr;
	
	program = new GeometryShaderProgram();
	//program = new UniformBufferProgram();
	
	const int result = program->run();
	delete program;
	return result;
}
