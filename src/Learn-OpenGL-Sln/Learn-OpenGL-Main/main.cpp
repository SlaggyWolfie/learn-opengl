#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <array>
#include "Shader.hpp"

const int INIT_ERROR = -1;
const unsigned int INITIAL_SCREEN_WIDTH = 800;
const unsigned int INITIAL_SCREEN_HEIGHT = 600;

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
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create window
	GLFWwindow* window = glfwCreateWindow(INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT, "My OpenGL Window!", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return INIT_ERROR;
	}

	glfwMakeContextCurrent(window);

	// Alternative suggestions by Resharper
	//if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	//if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))

	// Initialize GLAD — btw, black magic with this case as noted above
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD." << std::endl;
		return INIT_ERROR;
	}

	// Set viewport size within window and assign resize function
	glViewport(0, 0, INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	const Shader defaultShader("shaders/shader.vert", "shaders/shader.frag");
	const unsigned int shaderProgram = defaultShader.ID;
	
	// Basic rendering setup

	// Shape - Triangle
	float vertices[]
	{
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	//---------------------------------------------------------------------------//
	// > bind the Vertex Array Object first, then bind and set vertex buffer(s),
	// > and then configure vertex attributes(s). 
	// bind vertex array object
	glBindVertexArray(vao);

	// copy vertex array in a vertex buffer for OpenGL
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// set the vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// > note that this is allowed, the call to glVertexAttribPointer registered VBO
	// > as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// > You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO,
	// > but this rarely happens. Modifying other VAOs requires a call to glBindVertexArray
	// > anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// > uncomment this call to draw in wireframe polygon
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//---------------------------------------------------------------------------//

	// time-based shader implementation
	float timeValue = 0;
	float greenValue = 0;
	int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
	
	// Program Loop (Render Loop)
	while (!glfwWindowShouldClose(window))
	{
		timeValue = glfwGetTime();
		greenValue = sin(timeValue) / 2 + 0.5f; // slight normalization here.
		
		// input (obviously)
		process_input(window);

		// rendering
		glClearColor
		(
			_clearColor[0],
			_clearColor[1],
			_clearColor[2],
			_clearColor[3]
		);

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glUniform4f(vertexColorLocation, 0, greenValue, 0, 1);

		// > seeing as we only have a single VAO there's no need to bind it every time,
		// > but we'll do so to keep things a bit more organized
		glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		// > no need to unbind it every time 
		//glBindVertexArray(0);

		// double buffering, and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean-up!
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

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

void set_clear_color(const float r, const float g, const float b, const float a)
{
	_clearColor = { r, g, b, a };
}
