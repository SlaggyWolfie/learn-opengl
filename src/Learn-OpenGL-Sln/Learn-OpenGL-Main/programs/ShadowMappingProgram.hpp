#pragma once
#ifndef SHADOW_MAPPING_PROGRAM_HPP
#define SHADOW_MAPPING_PROGRAM_HPP

#include <programs/ReusedProgram.hpp>

class ShadowMappingProgram : public ReusedProgram
{
public:
	int run() override;
};
#endif