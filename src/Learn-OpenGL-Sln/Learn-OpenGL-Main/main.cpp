#include <iostream>
#include <algorithm>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Shader.hpp"
#include "Camera.hpp"
#include "LightAttenuationTerms.hpp"
#include "Model.hpp"

using color = glm::vec3;
using color4 = glm::vec4;

const int INIT_ERROR = -1;
const unsigned int INITIAL_SCREEN_WIDTH = 800;
const unsigned int INITIAL_SCREEN_HEIGHT = 600;

const float INITIAL_FOV = 45;

// green-ish color
const color4 _defaultClearColor{ 0.2f, 0.3f, 0.3f, 1.0f };
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

float matrix_minor(const float m[16], int r0, int r1, int r2, int c0, int c1, int c2);
void matrix_cofactor(const float src[16], float dst[16]);
unsigned int loadTexture(const std::string& path);

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args);
unsigned int loadCubemap(const std::vector<std::string>& pathsToTexturesFaces);

int main()
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

	// Create window
	GLFWwindow* window = glfwCreateWindow(INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT, "My OpenGL Window!", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return INIT_ERROR;
	}

	// Set viewport size within window and assign resize function
	//glViewport(0, 0, INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT);
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, lastMousePosition.x, lastMousePosition.y);

	// Initialize GLAD — btw, black magic with this case as noted above
	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
	{
		std::cout << "Failed to initialize GLAD." << std::endl;
		return INIT_ERROR;
	}

	// > configure global OpenGL state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_CULL_FACE);

	//stbi_set_flip_vertically_on_load(1);

	//----- end of setup -----//

	float cubeVertices[] =
	{
		// positions          // normals           // texture coordinates
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	const float planeVertices[] =
	{
		// > positions        // > texture Coords
							  // > (note we set these higher than 1
							  // > (together with GL_REPEAT as texture wrapping mode).
							  // > this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};

	const float transparentVertices[] =
	{
		// positions         // > texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};

	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	// bind vertex array object
	glBindVertexArray(cubeVAO);

	// copy vertex array in a vertex buffer for OpenGL
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// clean
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);

	// bind vertex array object
	glBindVertexArray(planeVAO);

	// copy vertex array in a vertex buffer for OpenGL
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normals attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// clean
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned int transparentVAO, transparentVBO;
	glGenVertexArrays(1, &transparentVAO);
	glGenBuffers(1, &transparentVBO);

	// bind vertex array object
	glBindVertexArray(transparentVAO);

	// copy vertex array in a vertex buffer for OpenGL
	glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normals attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// clean
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	std::vector<glm::vec3> transparentObjectsPositions;
	transparentObjectsPositions.emplace_back(-1.5f, 0.0f, -0.48f);
	transparentObjectsPositions.emplace_back(1.5f, 0.0f, 0.51f);
	transparentObjectsPositions.emplace_back(0.0f, 0.0f, 0.7f);
	transparentObjectsPositions.emplace_back(-0.3f, 0.0f, -2.3f);
	transparentObjectsPositions.emplace_back(0.5f, 0.0f, -0.6f);

	const glm::mat4 identity(1);
	glm::mat4 view, projection;
	glm::mat4 model = view = projection = identity;

	camera = new Camera(glm::vec3(0, 0, 3));

	//Shader unlit("shaders/unlit.vert", "shaders/unlit.frag");

	//const unsigned int cubeTextureID = loadTexture("assets/textures/marble.jpg");
	const unsigned int cubeTextureID = loadTexture("assets/textures/container.jpg");
	const unsigned int floorTextureID = loadTexture("assets/textures/metal.png");
	const unsigned int transparentTextureID = loadTexture("assets/textures/window.png");

	//unlit.use();
	//unlit.set("textureSampler", 0);

	std::vector<std::string> textureFaces
	{
		"assets/textures/skybox/right.jpg",
		"assets/textures/skybox/left.jpg",
		"assets/textures/skybox/top.jpg",
		"assets/textures/skybox/bottom.jpg",
		"assets/textures/skybox/front.jpg",
		"assets/textures/skybox/back.jpg"
	};

	// skybox = cubemap
	const unsigned int cubemapTextureID = loadCubemap(textureFaces);

	const float skyboxVertices[] =
	{
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	unsigned int cubemapVAO, cubemapVBO;
	glGenVertexArrays(1, &cubemapVAO);
	glGenBuffers(1, &cubemapVBO);

	glBindVertexArray(cubemapVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Model backpack("assets/objects/backpack/backpack.obj");

	const Shader skyboxShader("shaders/skybox");
	const Shader reflectiveShader("shaders/reflective");
	const Shader refractiveShader("shaders/refractive");

	skyboxShader.use();
	skyboxShader.set("cubemap", 0);

	const Shader& shader = refractiveShader;
	shader.use();
	shader.set("cubemap", 0);

	// Program Loop (Render Loop)
	while (!glfwWindowShouldClose(window))
	{
		const auto currentFrame = float(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Pre-input calculations | Critical
		view = camera->viewMatrix();
		projection = glm::perspective(glm::radians(camera->fov),
			float(INITIAL_SCREEN_WIDTH) / float(INITIAL_SCREEN_HEIGHT), 0.1f, 100.0f);

		//// farther objects are sorted to the back
		//std::map<float, glm::vec3> sortedTransparency;
		//for (auto transparentObjectsPosition : transparentObjectsPositions)
		//{
		//	const float distance = glm::distance(camera->position, transparentObjectsPosition);
		//	sortedTransparency[distance] = transparentObjectsPosition;
		//}

		process_input(window);

		// rendering
		glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//unlit.use();
		//unlit.set("view", view);
		//unlit.set("projection", projection);
		shader.use();
		shader.set("view", view);
		shader.set("projection", projection);
		shader.set("cameraPosition", camera->position);

		//glBindVertexArray(planeVAO);
		//glBindTexture(GL_TEXTURE_2D, floorTextureID);
		//glBindTexture(GL_TEXTURE_2D, cubemapTextureID);

		//unlit.use();
		//unlit.set("model", identity);
		shader.use();
		shader.set("model", identity);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		//glBindVertexArray(0);

		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, cubeTextureID);

		//unlit.set("model", identity);
		shader.set("model", identity);
		glBindTexture(GL_TEXTURE_2D, cubemapTextureID);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//model = glm::translate(identity, glm::vec3(-1, 0, -1));
		////unlit.set("model", model);
		//shader.set("model", model);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		//model = glm::translate(identity, glm::vec3(2, 0, 0));
		////unlit.set("model", model);
		//shader.set("model", model);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		//glBindVertexArray(transparentVAO);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, transparentTextureID);

		//unlit.use();
		//for (auto it = sortedTransparency.rbegin(); it != sortedTransparency.rend(); ++it)
		//{
		//	model = glm::translate(identity, it->second);
		//	unlit.set("model", model);
		//	glDrawArrays(GL_TRIANGLES, 0, 6);
		//}

		glDepthFunc(GL_LEQUAL);

		skyboxShader.use();
		skyboxShader.set("view", glm::mat4(glm::mat3(view)));
		skyboxShader.set("projection", projection);

		glBindVertexArray(cubemapVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS);

		// double buffering, and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean-up!
	// Scene
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteVertexArrays(1, &transparentVAO);

	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteBuffers(1, &transparentVBO);

	glDeleteTextures(1, &cubeTextureID);
	glDeleteTextures(1, &floorTextureID);
	glDeleteTextures(1, &transparentTextureID);

	// Cubemap
	glDeleteVertexArrays(1, &cubemapVAO);
	glDeleteBuffers(1, &cubemapVBO);
	glDeleteTextures(1, &cubemapTextureID);

	delete camera;

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
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		_clearColor = _defaultClearColor;

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		_clearColor = { 0.7f, 0, 0, 1 };

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mix_ratio += 0.01f;
		mix_ratio = std::min(mix_ratio, 1.0f);
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mix_ratio -= 0.01f;
		mix_ratio = std::max(mix_ratio, 0.0f);
	}

	unsigned int direction = 0;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) direction |= Camera::movement::FORWARD;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) direction |= Camera::movement::BACKWARD;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) direction |= Camera::movement::LEFT;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) direction |= Camera::movement::RIGHT;
	const bool shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

	if (direction)
		camera->processMovement(Camera::movement(direction), shiftPressed, deltaTime);
}

void mouse_callback(GLFWwindow*, const double x, const double y)
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

void scroll_callback(GLFWwindow*, const double, const double yOffset)
{
	camera->processZoom(float(yOffset));
}

// The following code is from: https://github.com/graphitemaster/normals_revisited
float matrix_minor(const float m[16], int r0, int r1, int r2, int c0, int c1, int c2)
{
	return
		m[4 * r0 + c0] * (m[4 * r1 + c1] * m[4 * r2 + c2] - m[4 * r2 + c1] * m[4 * r1 + c2]) -
		m[4 * r0 + c1] * (m[4 * r1 + c0] * m[4 * r2 + c2] - m[4 * r2 + c0] * m[4 * r1 + c2]) +
		m[4 * r0 + c2] * (m[4 * r1 + c0] * m[4 * r2 + c1] - m[4 * r2 + c0] * m[4 * r1 + c1]);
}

void matrix_cofactor(const float src[16], float dst[16])
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

// https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	const size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	if (size <= 0) { throw std::runtime_error("Error during formatting."); }
	const std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

unsigned int loadTexture(const std::string& path)
{
	unsigned int textureID = 0;
	glGenTextures(1, &textureID);

	int width, height, numberOfComponents;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &numberOfComponents, 0);
	if (data)
	{
		GLenum format;
		if (numberOfComponents == 1) format = GL_RED;
		else if (numberOfComponents == 3) format = GL_RGB;
		else if (numberOfComponents == 4) format = GL_RGBA;
		else format = GL_RGB;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//>  for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);

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

unsigned int loadCubemap(const std::vector<std::string>& pathsToTexturesFaces)
{
	unsigned int cubemapTextureID = 0;
	glGenTextures(1, &cubemapTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);

	int width, height;
	unsigned char* data;

	for (unsigned int i = 0; i < pathsToTexturesFaces.size(); ++i)
	{
		data = stbi_load(pathsToTexturesFaces[i].c_str(),
			&width, &height, nullptr, 0); // let's see if this crashes eventually

		if (data)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
				width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else
			std::cout << "Cubemap texture failed to load at path: " << pathsToTexturesFaces[i] << std::endl;

		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return cubemapTextureID;
}