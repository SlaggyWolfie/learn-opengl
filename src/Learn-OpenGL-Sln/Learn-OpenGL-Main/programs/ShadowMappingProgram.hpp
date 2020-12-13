#pragma once
#ifndef SHADOW_MAPPING_PROGRAM_HPP
#define SHADOW_MAPPING_PROGRAM_HPP

#include <programs/ReusedProgram.hpp>

class ShadowMappingProgram : public ReusedProgram
{
public:
	bool isGammaCorrectionOn = true;
	bool gammaKeyPressed = false;

	int run() override;
	void process_input(GLFWwindow* window) override;
};
#endif