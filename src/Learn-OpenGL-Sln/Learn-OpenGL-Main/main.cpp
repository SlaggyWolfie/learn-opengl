#include "programs/ExplodingModelProgram.hpp"

int main(int argc, char* argv[])
{
	Program* program = nullptr;
	
	program = new ExplodingModelProgram();
	
	const int result = program->run();
	delete program;
	return result;
}
