#include "programs/DisplayNormalsProgram.hpp"

int main(int argc, char* argv[])
{
	Program* program = nullptr;

	program = new InstancingProgram();

	const int result = program->run();
	delete program;
	return result;
}
