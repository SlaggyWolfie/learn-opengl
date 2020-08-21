#pragma once
#ifndef ASTEROID_FIELD_PROGRAM_HPP
#define ASTEROID_FIELD_PROGRAM_HPP

#include <programs/ReusedProgram.hpp>

class AsteroidFieldProgram : public ReusedProgram
{
public:
	int run() override;
};
#endif