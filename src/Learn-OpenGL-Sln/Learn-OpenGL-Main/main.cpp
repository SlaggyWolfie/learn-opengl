#include "programs/NativeMultisamplingProgram.hpp"
#include "programs/CustomMultisamplingProgram.hpp"

int main(int argc, char* argv[])
{
	Program* program = new CustomMultisamplingProgram();

	const int result = program->run();
	delete program;
	return result;
}
