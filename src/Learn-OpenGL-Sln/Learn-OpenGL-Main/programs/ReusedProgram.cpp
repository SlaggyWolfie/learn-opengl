#include "ReusedProgram.hpp"

#include <iostream>

#include "stb_image.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <engine/Shader.hpp>
#include <engine/Camera.hpp>
#include <engine/Model.hpp>
#include <glm/gtc/type_ptr.hpp>

bool ReusedProgram::initGLWindow(GLFWwindow*& window, int& errorCode)
{
	// Initialize GLFW context with OpenGL version 3.3 using the Core OpenGL profile
	glfwInit();

	//-----Setup-----//
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// MacOS-specific code
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT, "My OpenGL Window!", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		errorCode = INIT_ERROR;
		return true;
	}

	// Set viewport size within window and assign resize function
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);

	auto framebufferResize = [](GLFWwindow* window, const int w, const int h)
	{
		static_cast<ReusedProgram*>(glfwGetWindowUserPointer(window))->
			framebuffer_size_callback(window, w, h);
	};

	auto mouse = [](GLFWwindow* window, const double x, const double y)
	{
		static_cast<ReusedProgram*>(glfwGetWindowUserPointer(window))->
			mouse_callback(window, x, y);
	};

	auto scroll = [](GLFWwindow* window, const double x, const double y)
	{
		static_cast<ReusedProgram*>(glfwGetWindowUserPointer(window))->
			scroll_callback(window, x, y);
	};

	glfwSetFramebufferSizeCallback(window, framebufferResize);
	glfwSetCursorPosCallback(window, mouse);
	glfwSetScrollCallback(window, scroll);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, lastMousePosition.x, lastMousePosition.y);

	// Initialize GLAD � btw, black magic with this case as noted above
	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
	{
		std::cout << "Failed to initialize GLAD." << std::endl;
		errorCode = INIT_ERROR;
		return true;
	}

	// > configure global OpenGL state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_CULL_FACE);

	stbi_set_flip_vertically_on_load(1);
	return false;
}

void ReusedProgram::framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
	glViewport(0, 0, width, height);
}

void ReusedProgram::process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		_clearColor = _defaultClearColor;

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		_clearColor = { 0.7f, 0, 0, 1 };

	unsigned int direction = 0;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) direction |= Camera::movement::FORWARD;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) direction |= Camera::movement::BACKWARD;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) direction |= Camera::movement::LEFT;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) direction |= Camera::movement::RIGHT;
	const bool shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

	if (direction)
		camera->processMovement(Camera::movement(direction), shiftPressed, deltaTime);
}

void ReusedProgram::mouse_callback(GLFWwindow*, const double x, const double y)
{
	if (firstMouse)
	{
		lastMousePosition = glm::vec2(x, y);
		firstMouse = false;
	}

	const glm::vec2 mousePosition(x, y);
	glm::vec2 offset = mousePosition - lastMousePosition;
	lastMousePosition = mousePosition;

	// > reversed since y-coordinates range from bottom to top
	offset.y *= -1;

	camera->processOrientation(offset);
}

void ReusedProgram::scroll_callback(GLFWwindow*, const double, const double yOffset)
{
	camera->processZoom(float(yOffset));
}

// The following code is from: https://github.com/graphitemaster/normals_revisited
float ReusedProgram::matrix_minor(const float m[16], int r0, int r1, int r2, int c0, int c1, int c2)
{
	return
		m[4 * r0 + c0] * (m[4 * r1 + c1] * m[4 * r2 + c2] - m[4 * r2 + c1] * m[4 * r1 + c2]) -
		m[4 * r0 + c1] * (m[4 * r1 + c0] * m[4 * r2 + c2] - m[4 * r2 + c0] * m[4 * r1 + c2]) +
		m[4 * r0 + c2] * (m[4 * r1 + c0] * m[4 * r2 + c1] - m[4 * r2 + c0] * m[4 * r1 + c1]);
}

void ReusedProgram::matrix_cofactor(const float src[16], float dst[16])
{
	dst[0] = matrix_minor(src, 1, 2, 3, 1, 2, 3);
	dst[1] = -matrix_minor(src, 1, 2, 3, 0, 2, 3);
	dst[2] = matrix_minor(src, 1, 2, 3, 0, 1, 3);
	dst[3] = -matrix_minor(src, 1, 2, 3, 0, 1, 2);
	dst[4] = -matrix_minor(src, 0, 2, 3, 1, 2, 3);
	dst[5] = matrix_minor(src, 0, 2, 3, 0, 2, 3);
	dst[6] = -matrix_minor(src, 0, 2, 3, 0, 1, 3);
	dst[7] = matrix_minor(src, 0, 2, 3, 0, 1, 2);
	dst[8] = matrix_minor(src, 0, 1, 3, 1, 2, 3);
	dst[9] = -matrix_minor(src, 0, 1, 3, 0, 2, 3);
	dst[10] = matrix_minor(src, 0, 1, 3, 0, 1, 3);
	dst[11] = -matrix_minor(src, 0, 1, 3, 0, 1, 2);
	dst[12] = -matrix_minor(src, 0, 1, 2, 1, 2, 3);
	dst[13] = matrix_minor(src, 0, 1, 2, 0, 2, 3);
	dst[14] = -matrix_minor(src, 0, 1, 2, 0, 1, 3);
	dst[15] = matrix_minor(src, 0, 1, 2, 0, 1, 2);
}

unsigned int ReusedProgram::loadTexture(const std::string& path, const bool applyGammaCorrection)
{
	unsigned int textureID = 0;
	glGenTextures(1, &textureID);

	int width, height, numberOfComponents;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &numberOfComponents, 0);
	if (data)
	{
		GLenum internalFormat;
		GLenum dataFormat;
		if (numberOfComponents == 1)
		{
			internalFormat = dataFormat = GL_RED;
		}
		else if (numberOfComponents == 3)
		{
			internalFormat = applyGammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (numberOfComponents == 4)
		{
			internalFormat = applyGammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}
		else
		{
			internalFormat = dataFormat = GL_RGB;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//>  for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, dataFormat == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, dataFormat == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//float borderColor[] = { 1, 1, 0, 1 }; // brown-ish?
		//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
	}

	stbi_image_free(data);

	return textureID;
}