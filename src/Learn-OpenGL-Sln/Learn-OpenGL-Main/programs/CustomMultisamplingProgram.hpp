#pragma once
#ifndef CUSTOM_MULTISAMPLING_PROGRAM
#define CUSTOM_MULTISAMPLING_PROGRAM

#include "programs/ReusedProgram.hpp"

class CustomMultisamplingProgram : public ReusedProgram
{
public:
	int run() override;
};
#endif