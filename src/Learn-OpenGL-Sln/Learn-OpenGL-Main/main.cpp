#include "programs/LightingTestBlinnPhongProgram.hpp"
#include "programs/GammaCorrectionBlingPhonProgram.hpp"

int main(int argc, char* argv[])
{
	//Program* program = new LightingTestBlinnPhongProgram();
	Program* program = new GammaCorrectionBlingPhonProgram();

	const int result = program->run();
	delete program;
	return result;
}
