#pragma once
#ifndef LIGHTING_TEST_BLING_PHONG_PROGRAM_HPP
#define LIGHTING_TEST_BLING_PHONG_PROGRAM_HPP

#include <programs/ReusedProgram.hpp>

class LightingTestBlinnPhongProgram : public ReusedProgram
{
public:
	bool isBlinnOn = false;
	bool blinnKeyPressed = false;

	int run() override;
	void process_input(GLFWwindow* window) override;
};
#endif