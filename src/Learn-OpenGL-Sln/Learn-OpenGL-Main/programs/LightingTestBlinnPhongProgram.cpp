#include "LightingTestBlinnPhongProgram.hpp"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <engine/Shader.hpp>
#include <engine/Camera.hpp>
#include <glm/gtc/type_ptr.hpp>

int LightingTestBlinnPhongProgram::run()
{
	GLFWwindow* window;	int errorCode;
	if (initGLWindow(window, errorCode)) return errorCode;
	
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

	const unsigned floorTex = loadTexture("assets/textures/wood.png");

	const Shader shader("shaders/litBlinnPhong");
	shader.use();
	shader.set("textureSampler", floorTex);

	glm::vec3 lightPosition(0);

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
		shader.set("lightPosition", lightPosition);
		shader.set("isBlinn", isBlinnOn);

		glBindVertexArray(vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTex);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		std::cout << (isBlinnOn ? "Blinn-Phong" : "Phong") << std::endl;

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

void LightingTestBlinnPhongProgram::process_input(GLFWwindow* window)
{
	ReusedProgram::process_input(window);

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
	{
		isBlinnOn = !isBlinnOn;
		blinnKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
	{
		blinnKeyPressed = false;
	}
}
