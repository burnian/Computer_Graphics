/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 01/30/2020, 13:39
*@Last Modify: 11/04/2020, 23:44
*@Desc: point light shadow test
*********************************************************/
#include <iostream>
#include <vector>
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

#define SHADOW_WIDTH 1024.0f
#define SHADOW_HEIGHT 1024.0f
#define SHADOW_NEAR 1.0f
#define SHADOW_FAR 25.0f

//////////////////////////////
using namespace std;
using namespace glm;

//////////////////////////////
// global settings
//////////////////////////////
// frame interval correction parameters
GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame
// camera
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

// point light space transformation matrix, while directional light use orthogonal projection.
void GetPointLightSpaceTrans(vec3 pointLightWPos, vector<mat4>& pointLightSpaceTrans) {
	mat4 pointLightProj = perspective(radians(90.0f), SHADOW_WIDTH / SHADOW_HEIGHT, SHADOW_NEAR, SHADOW_FAR);
	// look at right, left, top, bottom, near and far, coordinates in function lookAt are based on world space.
	pointLightSpaceTrans.push_back(pointLightProj * lookAt(pointLightWPos, pointLightWPos + vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)));
	pointLightSpaceTrans.push_back(pointLightProj * lookAt(pointLightWPos, pointLightWPos + vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)));
	pointLightSpaceTrans.push_back(pointLightProj * lookAt(pointLightWPos, pointLightWPos + vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)));
	pointLightSpaceTrans.push_back(pointLightProj * lookAt(pointLightWPos, pointLightWPos + vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)));
	pointLightSpaceTrans.push_back(pointLightProj * lookAt(pointLightWPos, pointLightWPos + vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)));
	pointLightSpaceTrans.push_back(pointLightProj * lookAt(pointLightWPos, pointLightWPos + vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f)));
}

void RenderScene(const Shader& shader) {
	// floor
	shader.SetInt("material.diffuse", 0); // it will do nothing when the variable doesn't exist.
	shader.SetInt("material.specular", 0);
	shader.SetFloat("material.shininess", 32.0f);
	shader.SetInt("pointLight.depthCubemap", 3);
	mat4 model = mat4(1.0f);
	shader.SetMat4("model", model);
	utils::RenderPlane();

	// cubes
	shader.SetInt("material.diffuse", 1);
	shader.SetInt("material.specular", 2);
	shader.SetFloat("material.shininess", 64.0f);
	shader.SetInt("pointLight.depthCubemap", 3);
	model = mat4(1.0f);
	model = translate(model, vec3(0.0f, 1.5f, 0.0));
	model = scale(model, vec3(0.5f));
	shader.SetMat4("model", model);
	utils::RenderCube();

	model = mat4(1.0f);
	model = translate(model, vec3(2.0f, 0.0f, 1.0));
	model = scale(model, vec3(0.5f));
	shader.SetMat4("model", model);
	utils::RenderCube();

	model = mat4(1.0f);
	model = translate(model, vec3(-1.0f, 0.0f, 2.0));
	model = rotate(model, radians(60.0f), normalize(vec3(1.0, 0.0, 1.0)));
	model = scale(model, vec3(0.25));
	shader.SetMat4("model", model);
	utils::RenderCube();
}

//////////////////////////////
GLint main() {
	GLFWwindow* window = utils::InitWindow(SCR_WIDTH, SCR_HEIGHT, "PointLightShadow");
	if (window == nullptr) return -1;

	// callback binding
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, GLint w, GLint h) { glViewport(0, 0, w, h); }); // callback for window resizing.
	glfwSetCursorPosCallback(window, MouseCallback); // callback for cursor moving.
	// glfwSetScrollCallback(window, ScrollCallback); // callback for scrolling wheel.

	//////////////////////////////
	utils::BindTexture2D(GL_TEXTURE0, "res/texture/wood.png");
	utils::BindTexture2D(GL_TEXTURE1, "res/texture/container2.png");
	utils::BindTexture2D(GL_TEXTURE2, "res/texture/container2_specular.png");

	//////////////////////////////
	Shader depthCubemapShader("res/shader/depth_cubemap.vs", "res/shader/depth_cubemap.fs", "res/shader/depth_cubemap.gs");
	Shader BPPSShader("res/shader/blinn_phong_point_shadow.vs", "res/shader/blinn_phong_point_shadow.fs");

	//////////////////////////////
	// point light shadow depth cubemap
	ShadowMap shadowMap;
	shadowMap.InitCubeMap(SHADOW_WIDTH, SHADOW_HEIGHT, GL_TEXTURE3);

	vec3 pointLightWPos(0.0f, 3.0f, 0.0f);
	BPPSShader.SetPointLight(pointLightWPos, SHADOW_FAR, 1.0f, 3);

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

		// move light position over time
		//pointLightWPos.z = sin(currentFrame * 0.5) * 3.0;

		vector<mat4> pointLightSpaceTrans;
		GetPointLightSpaceTrans(pointLightWPos, pointLightSpaceTrans);

		//////////////////////////////
		// 1. render to depth map
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.fbo);
		glClear(GL_DEPTH_BUFFER_BIT);
		depthCubemapShader.Use();
		for (GLuint i = 0; i < 6; ++i)
			depthCubemapShader.SetMat4("shadowMatrices[" + to_string(i) + "]", pointLightSpaceTrans[i]);
		depthCubemapShader.SetFloat("farPlane", SHADOW_FAR);
		depthCubemapShader.SetVec3("lightWPos", pointLightWPos);
		RenderScene(depthCubemapShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//////////////////////////////
		// 2. render scene as normal using the generated depth/shadow map  
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		BPPSShader.Use();
		BPPSShader.SetVec3("viewWPos", camera.position);
		BPPSShader.SetMat4("projection", projection);
		BPPSShader.SetMat4("view", viewMat);
		BPPSShader.SetVec3("pointLight.position", pointLightWPos);
		RenderScene(BPPSShader);

		//////////////////////////////
		glfwSwapBuffers(window);
		// check and call events.
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}