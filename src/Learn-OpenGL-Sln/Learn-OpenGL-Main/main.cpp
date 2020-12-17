#include "programs/ShadowMappingProgram.hpp"
#include "programs/PointShadowsProgram.hpp"

int main(int argc, char* argv[])
{
	Program* program = new PointShadowsProgram();
	//Program* program = new ShadowMappingProgram();

	const int result = program->run();
	delete program;
	return result;
}
