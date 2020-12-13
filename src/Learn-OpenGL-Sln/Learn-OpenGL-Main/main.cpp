#include "programs/LightingTestBlinnPhongProgram.hpp"
#include "programs/GammaCorrectionBlinnPhongProgram.hpp"
#include "programs/ShadowMappingProgram.hpp"

int main(int argc, char* argv[])
{
	//Program* program = new LightingTestBlinnPhongProgram();
	Program* program = new ShadowMappingProgram();

	const int result = program->run();
	delete program;
	return result;
}
