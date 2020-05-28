#include <iostream>
#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "Shader.hpp"
#include "Camera.hpp"

using color = glm::vec3;
using uint = unsigned int;

const int INIT_ERROR = -1;
const unsigned int INITIAL_SCREEN_WIDTH = 800;
const unsigned int INITIAL_SCREEN_HEIGHT = 600;

const float INITIAL_FOV = 45;

// green-ish color
const std::array<float, 4> _defaultClearColor{ 0.2f, 0.3f, 0.3f, 1.0f };
std::array<float, 4> _clearColor = _defaultClearColor;

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void mouse_callback(GLFWwindow*, double x, double y);
void scroll_callback(GLFWwindow*, double, double yOffset);

int main()
{
	// Initialize GLFW context with OpenGL version 3.3 using the Core OpenGL profile
	glfwInit();

	//-----Setup window-----//
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

	//----- end of window setup -----//

	// Generate & configure GL texture
	unsigned int texID_container, texID_awesomeface;
	glGenTextures(1, &texID_container);
	glBindTexture(GL_TEXTURE_2D, texID_container);

	// Textures repeat (mirrored)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Textures clamp to the edge, and after that draw a color
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1, 1, 0, 1 }; // brown-ish?
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);

	// Load texture image data
	int width, height, numberChannels;
	unsigned char* data = stbi_load("assets/container.jpg", &width, &height, &numberChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "ERROR::TEXTURE::FAILED_TO_LOAD\n" << "assets/containers.jpg" << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);

	glGenTextures(1, &texID_awesomeface);
	glBindTexture(GL_TEXTURE_2D, texID_awesomeface);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("assets/awesomeface.png", &width, &height, &numberChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "ERROR::TEXTURE::FAILED_TO_LOAD\n" << "assets/awesomeface.png" << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);

	// Start initializing vertices, vaos, vbos, and such
	const Shader defaultShader("shaders/default.vert", "shaders/default.frag");

	// Basic rendering setup

	float vertices[] =
	{
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
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
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

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

	defaultShader.use();
	defaultShader.set("textureSampler1", 0);
	defaultShader.set("textureSampler2", 1);

	const glm::mat4 identity(1);
	glm::mat4 view = identity;
	glm::mat4 projection = identity;

	glEnable(GL_DEPTH_TEST);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, lastMousePosition.x, lastMousePosition.y);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	camera = new Camera(glm::vec3(1, 1, 5));
	camera->lookAt(glm::vec3(0));

	const color lightColor(1);
	const color objectColor(1, 0.5f, 0.31f);

	uint vaoLight = 0;
	glGenVertexArrays(1, &vaoLight);
	glBindVertexArray(vaoLight);
	// > we only need to bind the VBO, the container's buffer object already contains the data
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// > only position data for the lamp
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	glm::vec3 lightPosition(1.2f, 1, 2);
	const glm::vec3 lightScale(0.2f);

	const Shader lightShader("shaders/light.vert", "shaders/light.frag");
	const Shader litShader("shaders/lit.vert", "shaders/lit.frag");
	const Shader litViewShader("shaders/litPhongViewSpace.vert", "shaders/litPhongViewSpace.frag");
	const Shader litGouraud("shaders/litGouraud.vert", "shaders/litGouraud.frag");

	litShader.use();
	litShader.set("objectColor", objectColor);
	litShader.set("lightColor", lightColor);

	litViewShader.use();
	litViewShader.set("objectColor", objectColor);
	litViewShader.set("lightColor", lightColor);

	litGouraud.use();
	litGouraud.set("objectColor", objectColor);
	litGouraud.set("lightColor", lightColor);

	// Program Loop (Render Loop)
	while (!glfwWindowShouldClose(window))
	{
		const float currentFrame = float(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		view = camera->viewMatrix();
		projection = glm::perspective(glm::radians(camera->fov),
			float(INITIAL_SCREEN_WIDTH) / float(INITIAL_SCREEN_HEIGHT), 0.1f, 100.0f);

		lightPosition.x = 1 + sin(currentFrame) * 2;
		lightPosition.y = sin(currentFrame / 2);

		process_input(window);

		// rendering
		glClearColor
		(
			_clearColor[0],
			_clearColor[1],
			_clearColor[2],
			_clearColor[3]
		);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightShader.use();

		glm::mat4 lightModel = identity;
		lightModel = glm::translate(lightModel, lightPosition);
		lightModel = glm::scale(lightModel, lightScale);

		lightShader.set("model", lightModel);
		lightShader.set("view", view);
		lightShader.set("projection", projection);

		glBindVertexArray(vaoLight);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		litShader.use();

		glm::mat4 litModel = identity;
		litShader.set("model", litModel);
		litShader.set("view", view);
		litShader.set("projection", projection);
		litShader.set("mvp", projection * view * litModel);
		litShader.set("normalMatrix", glm::mat3(glm::transpose(glm::inverse(identity))));
		litShader.set("viewerPosition", camera->position);
		litShader.set("lightPosition", lightPosition);

		litViewShader.use();
		litViewShader.set("model", litModel);
		litViewShader.set("view", view);
		litViewShader.set("projection", projection);
		litViewShader.set("lightPosition", lightPosition);

		litGouraud.use();
		litGouraud.set("model", litModel);
		litGouraud.set("view", view);
		litGouraud.set("projection", projection);
		litGouraud.set("viewerPosition", camera->position);
		litGouraud.set("lightPosition", lightPosition);

		// > seeing as we only have a single VAO there's no need to bind it every time,
		// > but we'll do so to keep things a bit more organized
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		// > no need to unbind it every time 
		//glBindVertexArray(0);

		// double buffering, and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean-up!
	glDeleteVertexArrays(1, &vaoLight);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	//glDeleteBuffers(1, &ebo);

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

	if (direction)
		camera->processMovement(Camera::movement(direction), deltaTime);
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