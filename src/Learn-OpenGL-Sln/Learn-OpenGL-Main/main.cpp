#include "programs/LightingTestBlinnPhongProgram.hpp"

int main(int argc, char* argv[])
{
	Program* program = new LightingTestBlinnPhongProgram();

	const int result = program->run();
	delete program;
	return result;
}
