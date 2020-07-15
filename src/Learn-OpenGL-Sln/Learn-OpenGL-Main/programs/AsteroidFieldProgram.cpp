#include "AsteroidFieldProgram.hpp"

#include <iostream>

#include "stb_image.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <engine/Shader.hpp>
#include <engine/Camera.hpp>
#include <engine/Model.hpp>
#include <glm/gtc/type_ptr.hpp>

int AsteroidFieldProgram::run()
{
#pragma region init
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
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);

	auto framebufferResize = [](GLFWwindow* window, const int w, const int h)
	{
		static_cast<AsteroidFieldProgram*>(glfwGetWindowUserPointer(window))->
			framebuffer_size_callback(window, w, h);
	};

	auto mouse = [](GLFWwindow* window, const double x, const double y)
	{
		static_cast<AsteroidFieldProgram*>(glfwGetWindowUserPointer(window))->
			mouse_callback(window, x, y);
	};

	auto scroll = [](GLFWwindow* window, const double x, const double y)
	{
		static_cast<AsteroidFieldProgram*>(glfwGetWindowUserPointer(window))->
			scroll_callback(window, x, y);
	};

	glfwSetFramebufferSizeCallback(window, framebufferResize);
	glfwSetCursorPosCallback(window, mouse);
	glfwSetScrollCallback(window, scroll);

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

	stbi_set_flip_vertically_on_load(1);

	//----- end of setup -----//
#pragma endregion 

	const glm::mat4 identity(1);
	glm::mat4 view, projection;
	glm::mat4 model = view = projection = identity;

	camera = new Camera(glm::vec3(5, 5, 500));

	const unsigned amount = 500000;
	const auto modelMatrices = new glm::mat4[amount];
	srand(static_cast<unsigned>(glfwGetTime()));
	const float radius = 300;
	const float offset = 25.0f;

	for (unsigned i = 0; i < amount; ++i)
	{
		glm::mat4 model = identity;
		
		model = glm::rotate(model, 20.0f / 360.0f * glm::pi<float>(), glm::vec3(1, 0, 0));

		// > 1. translation: displace along circle with 'radius' in range [-offset, offset]
		const float angle = static_cast<float>(i) / amount * 360.0f;
		const auto randomDisplacement = [&offset]() { return static_cast<float>(rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset; };

		const float x = sin(angle) * radius + randomDisplacement();
		const float y = randomDisplacement() * 0.4f;
		const float z = cos(angle) * radius + randomDisplacement();

		model = glm::translate(model, glm::vec3(x, y, z));

		// > 2. scale: scale between 0.05 and 0.25f
		const float scale = static_cast<float>(rand() % 20) / 100.0f + 0.05f;
		model = glm::scale(model, glm::vec3(scale));

		// > 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		const auto rotationalAngle = static_cast<float>(rand() % 360);
		model = glm::rotate(model, rotationalAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// > 4. now add to list of matrices
		modelMatrices[i] = model;
	}

	const Model planet("assets/objects/planet/planet.obj");
	const Model rock("assets/objects/rock/rock.obj");

	unsigned instanceVBO;
	glGenBuffers(1, &instanceVBO);

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	const auto& meshes = rock.meshes();
	for (const auto& mesh : meshes)
	{
		const unsigned vao = mesh.vao();
		glBindVertexArray(vao);

		// vertex attributes
		const auto vec4size = sizeof(glm::vec4);

		for (unsigned j = 3; j < 7; ++j)
		{
			glEnableVertexAttribArray(j);
			glVertexAttribPointer(j, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (void*)((j - 3) * vec4size));
			glVertexAttribDivisor(j, 1);
		}

		glBindVertexArray(0);
	}

	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	const Shader planetShader("shaders/unlit2");
	const Shader rockShader("shaders/unlitInstanced");

	// Program Loop (Render Loop)
	double lastFrame = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		const auto currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Pre-input calculations | Critical
		view = camera->viewMatrix();
		projection = glm::perspective(glm::radians(camera->fov),
			float(INITIAL_SCREEN_WIDTH) / float(INITIAL_SCREEN_HEIGHT), 0.1f, 1000.0f);
		
		planetShader.use();
		planetShader.set("projection", projection);
		planetShader.set("view", view);
		
		rockShader.use();
		rockShader.set("projection", projection);
		rockShader.set("view", view);

		process_input(window);

		// rendering
		glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		model = identity;
		model = glm::rotate(model, 20.0f / 360.0f * glm::pi<float>(), glm::vec3(1, 0, 0));
		model = glm::translate(model, glm::vec3(0, -3, 0));
		model = glm::scale(model, glm::vec3(40));

		planetShader.use();
		planetShader.set("model", model);
		planet.draw(planetShader);

		rockShader.use();

		rockShader.set("material.texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rock.texturesLoaded()[0].id);
		
		for (const auto& mesh : meshes)
		{
			glBindVertexArray(mesh.vao());
			glDrawElementsInstanced(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}

		// double buffering, and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean-up!
	//glDeleteVertexArrays(1, &vao);
	//glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &instanceVBO);

	delete camera;

	glfwTerminate();
	return 0;
}

void AsteroidFieldProgram::framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
	glViewport(0, 0, width, height);
}

void AsteroidFieldProgram::process_input(GLFWwindow* window)
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

void AsteroidFieldProgram::mouse_callback(GLFWwindow*, const double x, const double y)
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

void AsteroidFieldProgram::scroll_callback(GLFWwindow*, const double, const double yOffset)
{
	camera->processZoom(float(yOffset));
}

// The following code is from: https://github.com/graphitemaster/normals_revisited
float AsteroidFieldProgram::matrix_minor(const float m[16], int r0, int r1, int r2, int c0, int c1, int c2)
{
	return
		m[4 * r0 + c0] * (m[4 * r1 + c1] * m[4 * r2 + c2] - m[4 * r2 + c1] * m[4 * r1 + c2]) -
		m[4 * r0 + c1] * (m[4 * r1 + c0] * m[4 * r2 + c2] - m[4 * r2 + c0] * m[4 * r1 + c2]) +
		m[4 * r0 + c2] * (m[4 * r1 + c0] * m[4 * r2 + c1] - m[4 * r2 + c0] * m[4 * r1 + c1]);
}

void AsteroidFieldProgram::matrix_cofactor(const float src[16], float dst[16])
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

unsigned int AsteroidFieldProgram::loadTexture(const std::string& path)
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