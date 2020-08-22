#pragma once
#ifndef NATIVE_MULTISAMPLING_PROGRAM
#define NATIVE_MULTISAMPLING_PROGRAM

#include "programs/ReusedProgram.hpp"

class NativeMultisamplingProgram : public ReusedProgram
{
public:
	int run() override;
};
#endif