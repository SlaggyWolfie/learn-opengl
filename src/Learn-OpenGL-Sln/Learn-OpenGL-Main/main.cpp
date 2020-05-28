#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Shader.hpp"
#include "Camera.hpp"
#include <algorithm>
#include "BasicMaterialLibrary.hpp"

using color = glm::vec3;
using color4 = glm::vec4;
using uint = unsigned int;

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void mouse_callback(GLFWwindow*, double x, double y);
void scroll_callback(GLFWwindow*, double, double yOffset);

float matrix_minor(const float m[16], int r0, int r1, int r2, int c0, int c1, int c2);
void matrix_cofactor(const float src[16], float dst[16]);
void parse_basic_material(const Shader& shader, const BasicMaterialLibrary::BasicMaterial& material);
unsigned int loadTexture(const std::string& path);

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

	glEnable(GL_DEPTH_TEST);

	//----- end of window setup -----//

	const unsigned int texID_diffuseMap = loadTexture("assets/container2.png");
	const unsigned int texID_specularMap = loadTexture("assets/container2_specular.png");

	float vertices[] =
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

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// normal attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	uint vaoLight = 0;
	glGenVertexArrays(1, &vaoLight);

	glBindVertexArray(vaoLight);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	const glm::mat4 identity(1);
	glm::mat4 view = identity;
	glm::mat4 projection = identity;

	//camera = new Camera(glm::vec3(1, 1, 5));
	//camera->lookAt(glm::vec3(0));

	camera = new Camera(glm::vec3(-2, -0.5f, 2.5f));
	camera->lookAt(glm::vec3(0.5f, 0.5f, 1));

	const color objectColor(1, 0.5f, 0.31f);

	glm::vec3 lightPosition(1.2f, 1, 2);
	const glm::vec3 lightScale(0.2f);

	const Shader lightShader("shaders/light.vert", "shaders/light.frag");
	const Shader litShader("shaders/lit.vert", "shaders/lit.frag");

	litShader.use();
	litShader.set("material.diffuse", 0);
	litShader.set("material.specular", 1);

	// Program Loop (Render Loop)
	while (!glfwWindowShouldClose(window))
	{
		const float currentFrame = float(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		view = camera->viewMatrix();
		projection = glm::perspective(glm::radians(camera->fov),
			float(INITIAL_SCREEN_WIDTH) / float(INITIAL_SCREEN_HEIGHT), 0.1f, 100.0f);

		//lightPosition.x = 1 + sin(currentFrame) * 2;
		//lightPosition.y = sin(currentFrame / 2);

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

		litShader.use();

		glm::mat4 litModel = identity;
		litShader.set("model", litModel);
		litShader.set("view", view);
		litShader.set("projection", projection);
		litShader.set("mvp", projection * view * litModel);

		litShader.set("viewerPosition", camera->position);
		litShader.set("material.shininess", 32.0f);

		color lightColor(1);
		//lightColor.r = sin(currentFrame * 2.0f);
		//lightColor.g = sin(currentFrame * 0.7f);
		//lightColor.b = sin(currentFrame * 1.3f);

		litShader.set("light.position", lightPosition);
		litShader.set("light.ambientColor", lightColor * glm::vec3(0.2f));
		litShader.set("light.diffuseColor", lightColor * glm::vec3(0.5f));
		litShader.set("light.specularColor", color(1));

		glm::mat4 normalMatrix(0);
		matrix_cofactor(
			glm::value_ptr(litModel),
			glm::value_ptr(normalMatrix));

		litShader.set("normalMatrix", glm::mat3(normalMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID_diffuseMap);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texID_specularMap);

		// > seeing as we only have a single VAO there's no need to bind it every time,
		// > but we'll do so to keep things a bit more organized
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lightShader.use();

		glm::mat4 lightModel = identity;
		lightModel = glm::translate(lightModel, lightPosition);
		lightModel = glm::scale(lightModel, lightScale);

		lightShader.set("mvp", projection * view * lightModel);
		lightShader.set("lightColor", lightColor);

		glBindVertexArray(vaoLight);
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

void parse_basic_material(const Shader& shader, const BasicMaterialLibrary::BasicMaterial& material)
{
	shader.use();
	shader.set("material.ambientColor", material.ambient);
	shader.set("material.diffuseColor", material.diffuse);
	shader.set("material.specularColor", material.specular);
	shader.set("material.shininess", material.shine);
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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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