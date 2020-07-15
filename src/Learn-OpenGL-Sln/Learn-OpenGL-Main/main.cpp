//#include "programs/DisplayNormalsProgram.hpp"
//#include "programs/InstancingProgram.hpp"
#include "programs/AsteroidFieldProgram.hpp"

int main(int argc, char* argv[])
{
	Program* program = new AsteroidFieldProgram();

	const int result = program->run();
	delete program;
	return result;
}
