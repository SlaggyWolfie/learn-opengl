#include "programs/DisplayNormalsProgram.hpp"

int main(int argc, char* argv[])
{
	Program* program = nullptr;

	program = new DisplayNormalsProgram();

	const int result = program->run();
	delete program;
	return result;
}
