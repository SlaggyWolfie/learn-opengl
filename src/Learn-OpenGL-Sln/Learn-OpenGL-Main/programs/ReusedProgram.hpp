#pragma once
#ifndef REUSED_PROGRAM_HPP
#define REUSED_PROGRAM_HPP
#include "Program.hpp"

#include <string>
#include <stdexcept>
#include <memory>

// ReSharper disable once CppUnusedIncludeDirective
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <helpers/LightAttenuationTerms.hpp>

class Camera;

class ReusedProgram : public Program
{
public:
	const int INIT_ERROR = -17;
	const int INITIAL_SCREEN_WIDTH = 800;
	const int INITIAL_SCREEN_HEIGHT = 600;

	// green-ish color
	const glm::vec4 _defaultClearColor{ 0.1f, 0.1f, 0.1f, 1.0f };
	glm::vec4 _clearColor = _defaultClearColor;

	double deltaTime = 0;
	bool firstMouse = true;

	glm::vec2 lastMousePosition = glm::vec2
	(
		float(INITIAL_SCREEN_WIDTH) / 2,
		float(INITIAL_SCREEN_HEIGHT) / 2
	);

	Camera* camera = nullptr;

	virtual bool initGLWindow(GLFWwindow*& window, int& errorCode);
	virtual void process_input(GLFWwindow* window);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow*, double x, double y);
	void scroll_callback(GLFWwindow*, double, double yOffset);

	static float matrix_minor(const float m[16], int r0, int r1, int r2, int c0, int c1, int c2);
	static void matrix_cofactor(const float src[16], float dst[16]);

	static unsigned int loadTexture(const std::string& path, bool applyGammaCorrection = false);

	// https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
	template<typename ... Args>
	static std::string string_format(const std::string& format, Args ... args)
	{
		const size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		if (size <= 0) { throw std::runtime_error("Error during formatting."); }
		const std::unique_ptr<char[]> buf(new char[size]);
		snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
	}
};
#endif