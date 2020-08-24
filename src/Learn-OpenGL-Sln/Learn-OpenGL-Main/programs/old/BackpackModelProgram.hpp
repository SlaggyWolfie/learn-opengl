#pragma once
#ifndef BACKPACK_MODEL_PROGRAM_HPP
#define BACKPACK_MODEL_PROGRAM_HPP

#include <programs/Program.hpp>

#include <stdexcept>
#include <memory>

// ReSharper disable once CppUnusedIncludeDirective
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <helpers/LightAttenuationTerms.hpp>

class Shader;
class Camera;

class BackpackModelProgram : public Program
{
public:
	using color = glm::vec3;
	using color4 = glm::vec4;
	using uint = unsigned int;

	const int INIT_ERROR = -1;
	const int INITIAL_SCREEN_WIDTH = 800;
	const int INITIAL_SCREEN_HEIGHT = 600;

	const float INITIAL_FOV = 45;

	// green-ish color
	const color4 _defaultClearColor = color4(0.2f, 0.3f, 0.3f, 1.0f);
	color4 _clearColor = _defaultClearColor;

	float mix_ratio = 0.2f;
	float deltaTime = 0;
	float lastFrame = 0;
	bool firstMouse = true;

	glm::vec2 lastMousePosition = glm::vec2
	(
		float(INITIAL_SCREEN_WIDTH) / 2,
		float(INITIAL_SCREEN_HEIGHT) / 2
	);

	Camera* camera = nullptr;
	LightAttenuationTerms light_attenuation_terms;

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void process_input(GLFWwindow* window);
	void mouse_callback(GLFWwindow*, double x, double y);
	void scroll_callback(GLFWwindow*, double, double yOffset);

	static float matrix_minor(const float m[16], int r0, int r1, int r2, int c0, int c1, int c2);
	static void matrix_cofactor(const float src[16], float dst[16]);
	void parse_light_attenuation(const Shader& shader, const std::string& attenuationAddress, const float distance);

	int run() override;

	BackpackModelProgram() = default;
	~BackpackModelProgram() override;

	BackpackModelProgram(const BackpackModelProgram&) = delete; // copy constructor
	BackpackModelProgram(const BackpackModelProgram&&) = delete; // move constructor
	BackpackModelProgram& operator= (const BackpackModelProgram&) = delete; // copy assignment operator
	BackpackModelProgram& operator= (const BackpackModelProgram&&) = delete; // move assignment operator

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
#endif // BACKPACK_MODEL_PROGRAM_HPP
