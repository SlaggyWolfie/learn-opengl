#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <array>

const int _init_error = -1;

// green-ish color
const std::array<float, 4> _defaultClearColor{ 0.2f, 0.3f, 0.3f, 1.0f };

std::array<float, 4> _clearColor = _defaultClearColor;

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height);
void process_input(GLFWwindow* window);
void set_clear_color(const std::array<float, 4>& color);
void set_clear_color(float r, float g, float b, float a);

int main()
{
	// Initialize GLFW context with OpenGL version 3.3 using the Core OpenGL profile
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// MacOS-specific code
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create window
	GLFWwindow* window = glfwCreateWindow(800, 600, "My OpenGL Window!", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return _init_error;
	}

	glfwMakeContextCurrent(window);

	// Alternative suggestions by Resharper
	//if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	//if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD." << std::endl;
		return _init_error;
	}

	// Set viewport size within window and assign resize function
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	while (!glfwWindowShouldClose(window))
	{
		process_input(window);

		glClearColor(
			_clearColor[0],
			_clearColor[1],
			_clearColor[2],
			_clearColor[3]
		);

		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		set_clear_color(_defaultClearColor);
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		// mildly dark red
		set_clear_color(0.7f, 0, 0, 1);
	}
}

void set_clear_color(const std::array<float, 4>& color)
{
	_clearColor = color;
}

void set_clear_color(float r, float g, float b, float a)
{
	_clearColor = { r, g, b, a };
}
