#pragma once
#ifndef POINT_SHADOWS_PROGRAM_HPP
#define POINT_SHADOWS_PROGRAM_HPP

#include <programs/ReusedProgram.hpp>

class Shader;

class PointShadowsProgram : public ReusedProgram
{
private:
	unsigned cubeVAO = 0;
	unsigned cubeVBO = 0;

	unsigned quadVAO = 0;
	unsigned quadVBO = 0;

	void renderScene(const Shader& shader);
	void renderCube();
	void renderQuad();
	
public:
	
	struct KeySettingHeld
	{
		bool setting = true;
		bool held = false;

		KeySettingHeld() = default;
		KeySettingHeld(const bool setting, const bool held) : setting(setting), held(held) { }
	};
	
	int run() override;
	void process_input(GLFWwindow* window) override;

	KeySettingHeld shadows;
};
#endif