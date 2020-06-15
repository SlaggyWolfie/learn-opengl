#include "programs/GeometryShaderProgram.hpp"

int main(int argc, char* argv[])
{
	Program* program = nullptr;
	
	program = new GeometryShaderProgram();
	
	const int result = program->run();
	delete program;
	return result;
}
