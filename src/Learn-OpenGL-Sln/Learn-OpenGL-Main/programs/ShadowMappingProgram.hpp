#pragma once
#ifndef SHADOW_MAPPING_PROGRAM_HPP
#define SHADOW_MAPPING_PROGRAM_HPP

#include <programs/ReusedProgram.hpp>

class Shader;

class ShadowMappingProgram : public ReusedProgram
{
private:
	unsigned cubeVAO = 0;
	unsigned cubeVBO = 0;

	unsigned quadVAO = 0;
	unsigned quadVBO = 0;

	void renderScene(const Shader& shader, unsigned planeVAO);
	void renderCube();
	void renderQuad();
	
public:
	int run() override;
};
#endif