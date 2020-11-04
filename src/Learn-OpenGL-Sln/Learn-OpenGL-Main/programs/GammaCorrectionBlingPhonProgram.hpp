#pragma once
#ifndef GAMMA_CORRECTION_BLING_PHONG_PROGRAM_HPP
#define GAMMA_CORRECTION_BLING_PHONG_PROGRAM_HPP

#include <programs/ReusedProgram.hpp>

class GammaCorrectionBlingPhonProgram : public ReusedProgram
{
public:
	bool isGammaCorrectionOn = true;
	bool gammaKeyPressed = false;

	int run() override;
	void process_input(GLFWwindow* window) override;
};
#endif