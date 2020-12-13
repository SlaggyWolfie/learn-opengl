#include "GammaCorrectionBlinnPhongProgram.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <engine/Shader.hpp>
#include <engine/Camera.hpp>
#include <glm/gtc/type_ptr.hpp>

int GammaCorrectionBlinnPhongProgram::run()
{
	GLFWwindow* window;	int errorCode;
	if (initGLWindow(window, errorCode)) return errorCode;

	//glEnable(GL_FRAMEBUFFER_SRGB);

	camera = new Camera(glm::vec3(0, 0, 3));

	const glm::mat4 identity(1);
	glm::mat4 view, projection;
	glm::mat4 model = view = projection = identity;

	const float planeVertices[] =
	{
		// positions            // normals         // texcoords
		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
		 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
	};

	unsigned vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof planeVertices, &planeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	glBindVertexArray(0);

	const unsigned floorTex = loadTexture("assets/textures/wood.png", false);
	const unsigned floorTexGammaCorrected = loadTexture("assets/textures/wood.png", true);

	const Shader shader("shaders/litBlinnPhongGamma");
	shader.use();
	shader.set("textureSampler", 0);

	const unsigned maxLights = 4;
	const glm::vec3 lightPositions[] =
	{
		glm::vec3(-3.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(3.0f, 0.0f, 0.0f)
	};

	const glm::vec3 lightColors[] =
	{
		glm::vec3(0.25),
		glm::vec3(0.50),
		glm::vec3(0.75),
		glm::vec3(1.00)
	};

	glUniform3fv(glGetUniformLocation(shader.id, "lightPositions"), maxLights, &lightPositions[0][0]);
	glUniform3fv(glGetUniformLocation(shader.id, "lightColors"), maxLights, &lightColors[0][0]);

	// Program Loop (Render Loop)
	double lastFrame = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		const auto currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		process_input(window);

		// Pre-input calculations | Critical
		view = camera->viewMatrix();
		projection = glm::perspective(glm::radians(camera->fov),
			float(INITIAL_SCREEN_WIDTH) / float(INITIAL_SCREEN_HEIGHT), 0.1f, 1000.0f);

		// rendering
		glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		shader.set("model", model);
		shader.set("view", view);
		shader.set("projection", projection);

		shader.set("viewPosition", camera->position);
		shader.set("gamma", isGammaCorrectionOn);

		glBindVertexArray(vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, isGammaCorrectionOn ? floorTexGammaCorrected : floorTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// double buffering, and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean-up!
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	delete camera;

	glfwTerminate();
	return 0;
}

void GammaCorrectionBlinnPhongProgram::process_input(GLFWwindow* window)
{
	ReusedProgram::process_input(window);

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !gammaKeyPressed)
	{
		isGammaCorrectionOn = !isGammaCorrectionOn;
		gammaKeyPressed = true;

		std::cout << (isGammaCorrectionOn ? "Gamma enabled" : "Gamma disabled") << std::endl;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
	{
		gammaKeyPressed = false;
	}
}
