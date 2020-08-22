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
	GLFWwindow* window;	int errorCode;
	if (initGLWindow(window, errorCode)) return errorCode;

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
