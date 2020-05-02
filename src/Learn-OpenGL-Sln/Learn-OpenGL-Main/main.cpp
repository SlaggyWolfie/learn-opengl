#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <array>

const int INIT_ERROR = -1;
const unsigned int INITIAL_SCREEN_WIDTH = 800;
const unsigned int INITIAL_SCREEN_HEIGHT = 600;

// green-ish color
const std::array<float, 4> _defaultClearColor{ 0.2f, 0.3f, 0.3f, 1.0f };

std::array<float, 4> _clearColor = _defaultClearColor;


const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";


const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

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

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD." << std::endl;
		return INIT_ERROR;
	}

	// Set viewport size within window and assign resize function
	glViewport(0, 0, INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Vertex Shader
	const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	// check for vertex errors
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment Shader
	const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	// check for fragment errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Shader Linking
	const unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cout << "ERROR:SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Basic rendering setup

	//// Shape - Triangle
	//float vertices[]
	//{
	//	-0.5f, -0.5f, 0.0f,
	//	 0.5f, -0.5f, 0.0f,
	//	 0.0f,  0.5f, 0.0f
	//};

	//// Shape - Rectangle (but 2 Tri's)
	//float vertices[]
	//{
	//	// > first triangle
	//	 0.5f,  0.5f, 0.0f,  // > top right
	//	 0.5f, -0.5f, 0.0f,  // > bottom right
	//	-0.5f,  0.5f, 0.0f,  // > top left 
	//	// > second triangle
	//	 0.5f, -0.5f, 0.0f,  // > bottom right
	//	-0.5f, -0.5f, 0.0f,  // > bottom left
	//	-0.5f,  0.5f, 0.0f   // > top left
	//};

	// Shape - Rect (4 points)
	float vertices[]
	{
		 0.5f,  0.5f, 0.0f,  // > top right
		 0.5f, -0.5f, 0.0f,  // > bottom right
		-0.5f, -0.5f, 0.0f,  // > bottom left
		-0.5f,  0.5f, 0.0f   // > top left 
	};

	// > note that we start from 0!
	unsigned int indices[]
	{
		0, 1, 3,   // > first triangle
		1, 2, 3    // > second triangle
	};

	unsigned int vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	//---------------------------------------------------------------------------//
	// > bind the Vertex Array Object first, then bind and set vertex buffer(s),
	// > and then configure vertex attributes(s). 
	// bind vertex array object
	glBindVertexArray(vao);

	// copy vertex array in a vertex buffer for OpenGL
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// copy index array in an element buffer for OpenGL
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

	// Program Loop (Render Loop)
	while (!glfwWindowShouldClose(window))
	{
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

		// > seeing as we only have a single VAO there's no need to bind it every time,
		// > but we'll do so to keep things a bit more organized
		glBindVertexArray(vao);
		
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		// > no need to unbind it every time 
		glBindVertexArray(0);

		// double buffering, and poll IO events
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

void set_clear_color(const float r, const float g, const float b, const float a)
{
	_clearColor = { r, g, b, a };
}
