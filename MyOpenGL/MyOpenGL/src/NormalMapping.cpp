/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 11/26/2020, 00:15
*@Last Modify: 11/26/2020, 00:15
*@Desc: normal mapping test
*********************************************************/
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils.h"
#include "Shader.h"
#include "Camera.h"
#include "ShadowMap.hpp"

//////////////////////////////
#define SCR_WIDTH 1280.0f
#define SCR_HEIGHT 720.0f
#define SCR_NEAR 0.1f
#define SCR_FAR 100.0f

//////////////////////////////
using namespace glm;

//////////////////////////////
// global settings
// frame interval correction parameters
GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame
Camera camera(vec3(-2.0f, 0.0f, 5.0f), vec3(0.0f, 1.0f, 0.0f), -60.0f);
mat4 projection = perspective(radians(camera.fov), SCR_WIDTH / SCR_HEIGHT, SCR_NEAR, SCR_FAR);

//////////////////////////////
void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera::FORWARD, deltaTime);
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera::BACKWARD, deltaTime);
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera::LEFT, deltaTime);
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera::RIGHT, deltaTime);
}

//////////////////////////////
GLfloat lastX = 400, lastY = 300;
GLboolean isFirstMouse = true;
void MouseCallback(GLFWwindow* window, GLdouble xpos, GLdouble ypos) {
	if (isFirstMouse) { // initially set to true
		lastX = xpos;
		lastY = ypos;
		isFirstMouse = false;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = ypos - lastY; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

//////////////////////////////
GLint main() {
	GLFWwindow* window = utils::InitWindow(SCR_WIDTH, SCR_HEIGHT, "NormalMapping");
	if (window == nullptr) return -1;

	// callback binding
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, GLint w, GLint h) { glViewport(0, 0, w, h); }); // callback for window resizing.
	glfwSetCursorPosCallback(window, MouseCallback); // callback for cursor moving.
	// glfwSetScrollCallback(window, ScrollCallback); // callback for scrolling wheel.

	//////////////////////////////
	utils::BindTexture2D(GL_TEXTURE0, "res/texture/brickwall/brickwall.jpg");
	utils::BindTexture2D(GL_TEXTURE1, "res/texture/brickwall/brickwall_normal.jpg");

	//////////////////////////////
	Shader normalMapShader("res/shader/normal_mapping.vs", "res/shader/normal_mapping.fs");
	normalMapShader.Use();
	normalMapShader.SetInt("material.diffuse", 0); // it will do nothing when the variable doesn't exist.
	normalMapShader.SetInt("material.normal", 1);

	//////////////////////////////
	vec3 lightPosWS(0.5f, 1.0f, 0.3f);

	//////////////////////////////
	// render loop
	mat4 model;
	while (!glfwWindowShouldClose(window)) {
		ProcessInput(window);
		// frame ratio correction
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// fill the certain buffers with the specified value.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//////////////////////////////
		mat4 viewMat = camera.GetViewMatrix();
		normalMapShader.Use();
		normalMapShader.SetMat4("projection", projection);
		normalMapShader.SetMat4("view", viewMat);
		// render normal-mapped quad
		model = mat4(1.0f);
		// rotate the quad to show normal mapping from multiple directions
		model = rotate(model, radians((float)glfwGetTime() * -10.0f), normalize(vec3(1.0, 0.0, 1.0)));
		normalMapShader.SetMat4("model", model);
		normalMapShader.SetVec3("viewPosWS", camera.position);
		normalMapShader.SetVec3("lightPosWS", lightPosWS);
		utils::RenderQuadNM();

		// render light source (simply re-renders a smaller plane at the light's position for debugging/visualization)
		model = mat4(1.0f);
		model = translate(model, lightPosWS);
		model = scale(model, vec3(0.1f));
		normalMapShader.SetMat4("model", model);
		utils::RenderQuadNM();

		//////////////////////////////
		glfwSwapBuffers(window);
		// check and call events.
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}