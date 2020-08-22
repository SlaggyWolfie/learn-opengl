#include "programs/NativeMultisamplingProgram.hpp"

int main(int argc, char* argv[])
{
	Program* program = new NativeMultisamplingProgram();

	const int result = program->run();
	delete program;
	return result;
}
