#include "PointShadowsProgram.hpp"

#include <iostream>
#include <array>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <engine/Shader.hpp>
#include <engine/Camera.hpp>

int PointShadowsProgram::run()
{
	GLFWwindow* window;	int errorCode;
	if (initGLWindow(window, errorCode)) return errorCode;

	//glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);

	camera = new Camera(glm::vec3(0, 0, 3));

	const glm::mat4 identity(1);
	glm::mat4 view, projection;
	glm::mat4 model = view = projection = identity;

	const unsigned SHADOW_TEX_WIDTH = 1024, SHADOW_TEX_HEIGHT = 1024;

	//unsigned depthMapTex;
	//glGenTextures(1, &depthMapTex);
	//glBindTexture(GL_TEXTURE_2D, depthMapTex);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_TEX_WIDTH, SHADOW_TEX_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//const float borderColor[] = { 1, 1, 1, 1 };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	unsigned depthCubemapTex;
	glGenTextures(1, &depthCubemapTex);

	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemapTex);
	for (unsigned i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_TEX_WIDTH, SHADOW_TEX_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);


	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	unsigned depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemapTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm::vec3 lightPosition(0);

	const float nearPlane = 1.f, farPlane = 25.f;
	const float aspect = static_cast<float>(SHADOW_TEX_WIDTH) / static_cast<float>(SHADOW_TEX_HEIGHT);
	//const glm::mat4 lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, nearPlane, farPlane);
	const glm::mat4 lightProjection = glm::perspective(glm::radians(90.f), aspect, nearPlane, farPlane);
	const glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0), glm::vec3(0, 1, 0));

	const glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	std::array<glm::mat4, 6> shadowTransforms{ };
	auto setShadowTransform = [&](const unsigned index, const glm::vec3 forward, const glm::vec3 up)
	{
		shadowTransforms[index] = lightProjection * glm::lookAt(lightPosition, lightPosition + forward, up);
	};
	
	const unsigned floorTex = loadTexture("assets/textures/wood.png", false);

	const Shader shader("shaders/litPointShadows");
	shader.use();
	shader.set("textureSampler", 0);
	shader.set("cubeShadowMap", 1);

	shader.set("farPlane", farPlane);

	const Shader cubeDepthShader(
		"shaders/cubeDepthShader.vert",
		"shaders/cubeDepthShader.geom",
		"shaders/cubeDepthShader.frag"
	);

	cubeDepthShader.use();
	cubeDepthShader.set("farPlane", farPlane);

	//const Shader debugDepthQuad("shaders/debugDepthQuad");
	//debugDepthQuad.use();
	//debugDepthQuad.set("textureSampler", 0);
	//debugDepthQuad.set("nearPlane", nearPlane);
	//debugDepthQuad.set("farPlane", farPlane);

	// Program Loop (Render Loop)
	double lastFrame = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		const auto currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		process_input(window);

		lightPosition.z = sin(currentFrame * 0.5) * 3;

		// 0. create depth cubemap transformation matrices
		setShadowTransform(0, glm::vec3(1, 0, 0), glm::vec3(0, -1, 0));
		setShadowTransform(1, glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0));
		setShadowTransform(2, glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
		setShadowTransform(3, glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
		setShadowTransform(4, glm::vec3(0, 0, 1), glm::vec3(0, -1, 0));
		setShadowTransform(5, glm::vec3(0, 0, -1), glm::vec3(0, -1, 0));

		// 1. render to depth map
		glViewport(0, 0, SHADOW_TEX_WIDTH, SHADOW_TEX_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

		glClear(GL_DEPTH_BUFFER_BIT);

		cubeDepthShader.use();
		cubeDepthShader.set("lightPosition", lightPosition);
		for (unsigned i = 0; i < 6; ++i)
			cubeDepthShader.set(std::string("shadowMatrices[").append(std::to_string(i)).append("]"),
				shadowTransforms[i]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTex);
		renderScene(cubeDepthShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. render scene normally but with shadows
		glViewport(0, 0, INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT);
		glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		view = camera->viewMatrix();
		projection = glm::perspective(glm::radians(camera->fov),
			float(INITIAL_SCREEN_WIDTH) / float(INITIAL_SCREEN_HEIGHT), 0.1f, 1000.0f);

		shader.use();
		shader.set("view", view);
		shader.set("projection", projection);
		shader.set("viewPosition", camera->position);
		shader.set("lightPosition", lightPosition);
		shader.set("shadowsOn", shadows.setting);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemapTex);

		renderScene(shader);

		//// 2.1 render depth map to quad for debugging
		//debugDepthQuad.use();
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, depthMapTex);
		////renderQuad();

		// double buffering, and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean-up!
	// should probably clean the Depth arrays and Textures and stuff, but eh...

	delete camera;

	glfwTerminate();
	return 0;
}

void PointShadowsProgram::process_input(GLFWwindow* window)
{
	ReusedProgram::process_input(window);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !shadows.held)
	{
		shadows.setting = !shadows.setting;
		shadows.held = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		shadows.held = false;
	}
}

void PointShadowsProgram::renderScene(const Shader& shader)
{
	glm::mat4 model = glm::mat4(1);

	auto set = [&]() { shader.set("model", model); };
	auto identity = [&]() { model = glm::mat4(1); };
	auto translate = [&](const glm::vec3& v) { model = glm::translate(model, v); };
	auto scalev = [&](const glm::vec3& v) { model = glm::scale(model, v); };
	auto scale = [&](const float s) { scalev(glm::vec3(s)); };

	scale(5);
	set();

	// > that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
	// > A small little hack to invert normals when drawing cube from the inside so lighting still works. and of course disable it
	glDisable(GL_CULL_FACE);
	shader.set("reverseNormals", 1);
	renderCube();
	shader.set("reverseNormals", 0);
	glEnable(GL_CULL_FACE);

	// cubes
	// 1
	identity();
	translate(glm::vec3(4, -3.5f, 0));
	scale(0.5f);
	set();
	renderCube();

	// 2
	identity();
	translate(glm::vec3(2, 3, 1));
	scale(0.75f);
	set();
	renderCube();

	// 3
	identity();
	translate(glm::vec3(-3, -1, 0));
	scale(0.5f);
	set();
	renderCube();

	// 4
	identity();
	translate(glm::vec3(-1.5f, 1, 1.5f));
	scale(0.5f);
	set();
	renderCube();

	// 5
	identity();
	translate(glm::vec3(-1.5f, 2, -3));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1, 0, 1)));
	scale(0.75f);
	set();
	renderCube();
}

void PointShadowsProgram::renderCube()
{
	// init
	if (!cubeVAO)
	{
		float vertices[] =
		{
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};

		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);

		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void PointShadowsProgram::renderQuad()
{
	// init
	if (!quadVAO)
	{
		float vertices[] =
		{
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);

		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(quadVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}