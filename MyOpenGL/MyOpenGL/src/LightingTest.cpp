/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 01/30/2020, 13:39
*@Last Modify: 10/06/2020, 22:59
*@Desc: directional light shadow test
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

//////////////////////////////
#define SCR_WIDTH 1280.0f
#define SCR_HEIGHT 720.0f
#define SCR_NEAR 0.1f
#define SCR_FAR 100.0f

#define SHADOW_WIDTH 1024.0f
#define SHADOW_HEIGHT 1024.0f
#define SHADOW_NEAR 1.0f
#define SHADOW_FAR 7.5f

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

bool spotLightOn = true;
bool isSpotBtnReleased = true;

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
	// spot light
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && isSpotBtnReleased) {
		spotLightOn = !spotLightOn;
		isSpotBtnReleased = false;
	}
	else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE && !isSpotBtnReleased) {
		isSpotBtnReleased = true;
	}
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

void RenderScene(const Shader& shader) {
	// floor
	shader.SetInt("material.diffuse", 0); // it will do nothing when the variable doesn't exist.
	shader.SetInt("material.specular", 0);
	shader.SetFloat("material.shininess", 32.0f);
	shader.SetInt("dirLight.depthMap", 3);
	mat4 model = mat4(1.0f);
	shader.SetMat4("model", model);
	utils::RenderPlane();

	// cubes
	shader.SetInt("material.diffuse", 1);
	shader.SetInt("material.specular", 2);
	shader.SetFloat("material.shininess", 64.0f);
	shader.SetInt("dirLight.depthMap", 3);
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
	GLFWwindow* window = utils::InitWindow(SCR_WIDTH, SCR_HEIGHT, "LightingTest");
	if (window == nullptr) return -1;

	// callback binding
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, GLint w, GLint h) { glViewport(0, 0, w, h); }); // callback for window resizing.
	glfwSetCursorPosCallback(window, MouseCallback); // callback for cursor moving.
	// glfwSetScrollCallback(window, ScrollCallback); // callback for scrolling wheel.

	//////////////////////////////
	vec3 cubePositions[] = {
		vec3( 0.0f,  0.0f,  0.0f),
		vec3( 2.0f,  5.0f, -15.0f),
		vec3(-1.5f, -2.2f, -2.5f),
		vec3(-3.8f, -2.0f, -12.3f),
		vec3( 2.4f, -0.4f, -3.5f),
		vec3(-1.7f,  3.0f, -7.5f),
		vec3( 1.3f, -2.0f, -2.5f),
		vec3( 1.5f,  2.0f, -2.5f),
		vec3( 1.5f,  0.2f, -1.5f),
		vec3(-1.3f,  1.0f, -1.5f)
	};

	vec3 pointLightPositions[] = {
		vec3( 0.7f,  0.2f,  2.0f),
		vec3( 2.3f, -3.3f, -4.0f),
		vec3(-4.0f,  2.0f, -12.0f),
		vec3( 0.0f,  0.0f, -3.0f)
	};

	utils::BindTexture2D(GL_TEXTURE0, "res/texture/wood.png");
	utils::BindTexture2D(GL_TEXTURE1, "res/texture/container2.png");
	utils::BindTexture2D(GL_TEXTURE2, "res/texture/container2_specular.png");

	// directional light space transformation matrix
	vec3 dirLightWPos = vec3(-2.0f, 4.0f, -1.0f);
	mat4 dirLightProj = ortho(-10.0f, 10.0f, -10.0f, 10.0f, SHADOW_NEAR, SHADOW_FAR);
	//mat4 dirLightProj = perspective(radians(45.0f), SHADOW_WIDTH / SHADOW_HEIGHT, DIR_SHADOW_NEAR, DIR_SHADOW_FAR); // none directional light
	mat4 dirLightViewTrans = lookAt(dirLightWPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 dirLightSpaceMat = dirLightProj * dirLightViewTrans;

	//////////////////////////////
	Shader depthMapShader("res/shader/depth_map.vs", "res/shader/depth_map.fs");
	//Shader depthMapDebugShader("res/shader/depth_map_debug.vs", "res/shader/depth_map_debug.fs");

	Shader BPSShader("res/shader/blinn_phong_shadow.vs", "res/shader/blinn_phong_shadow.fs");

	//for (GLint i = 0; i < 4; i++) {
	//	BPSShader.SetVec3("pointLights[" + to_string(i) + "].position", pointLightPositions[i]);
	//	BPSShader.SetVec3("pointLights[" + to_string(i) + "].ambient", 0.05f, 0.05f, 0.05f);
	//	BPSShader.SetVec3("pointLights[" + to_string(i) + "].diffuse", 0.8f, 0.8f, 0.8f);
	//	BPSShader.SetVec3("pointLights[" + to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
	//	BPSShader.SetFloat("pointLights[" + to_string(i) + "].constant", 1.0f);
	//	BPSShader.SetFloat("pointLights[" + to_string(i) + "].linear", 0.09f);
	//	BPSShader.SetFloat("pointLights[" + to_string(i) + "].quadratic", 0.032f);
	//}
	// lamp
	//Shader lampShader("res/shader/lamp.vs", "res/shader/lamp.fs");
	//lampShader.Use();
	//lampShader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);

	//////////////////////////////
	// directional light shadow depth texture
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	// we only care about depth, so the texture storing type sets to GL_DEPTH_COMPONENT
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// Solving the problem that an object is shadowed with nothing occluding it by setting the marginal value of the shadow depth texture to 1.
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	BPSShader.SetDirLight(vec3(0.0f) - dirLightWPos, 1.0f, 3);

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

		// 1. render to depth map
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glCullFace(GL_FRONT); // this may solve the problem that marginal shadow out of bounds, but in practice, it's not obvious.
		depthMapShader.Use();
		depthMapShader.SetMat4("lightSpaceMat", dirLightSpaceMat);
		RenderScene(depthMapShader);
		//glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. render scene as normal using the generated depth/shadow map  
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		BPSShader.Use();
		BPSShader.SetVec3("viewWPos", camera.position);
		BPSShader.SetMat4("projection", projection);
		BPSShader.SetMat4("view", viewMat);
		BPSShader.SetMat4("lightSpaceMat", dirLightSpaceMat);
		RenderScene(BPSShader);

		// show the shadow depth texture
		//depthMapDebugShader.Use();
		//depthMapDebugShader.SetFloat("SCR_NEAR", DIR_SHADOW_NEAR);
		//depthMapDebugShader.SetFloat("SCR_FAR", DIR_SHADOW_FAR);
		//depthMapDebugShader.SetInt("dirLight.depthMap", 0);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, depthMap);
		//utils::RenderQuad();

		// The lamp rotates around a fixed point
		//double sita = radians(sin(currentFrame) * 180);
		//lightWPos.x = cos(sita);
		//lightWPos.z = sin(sita);
		// light color variation
		//vec3 lightColor;
		//lightColor.x = fmax(sin(currentFrame * 2.0f), 0.0f);
		//lightColor.y = fmax(sin(currentFrame * 0.7f), 0.0f);
		//lightColor.z = fmax(sin(currentFrame * 1.3f), 0.0f);
		//vec3 diffuseColor = lightColor * vec3(0.9f);
		//vec3 ambientColor = diffuseColor * vec3(0.7f);

		// cube
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, containerTexture);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, containerSpecTexture);
		//glBindVertexArray(cubeVAO);
		//objectShader.Use();
		//objectShader.SetInt("spotLightOn", spotLightOn);
		//objectShader.SetVec3("viewWPos", camera.position);
		//objectShader.SetMat4("view", viewMat);
		//objectShader.SetVec3("spotLight.position", camera.position);
		//objectShader.SetVec3("spotLight.direction", camera.GetFront());
		//for (GLint i = 0; i < 10; i++) {
		//	mat4 model = mat4(1.0f);
		//	model = translate(model, cubePositions[i]);
		//	model = rotate(model, radians(20.0f * i), vec3(1.0f, 0.3f, 0.5f));
		//	objectShader.SetMat4("model", model);
		//	glDrawArrays(GL_TRIANGLES, 0, 36);
		//}

		// lamp
		//lightingShader.Use();
		//lightingShader.SetMat4("view", viewMat);
		//glBindVertexArray(lightVAO);
		//for (GLint i = 0; i < 4; i++) {
		//	mat4 model = mat4(1.0f);
		//	model = translate(model, pointLightPositions[i]);
		//	model = scale(model, vec3(0.2f));
		//	lightingShader.SetMat4("model", model);
		//	glDrawArrays(GL_TRIANGLES, 0, 36);
		//}

		//////////////////////////////
		glfwSwapBuffers(window);
		// check and call events.
		glfwPollEvents();
	}

	//glDeleteVertexArrays(1, &cubeVAO);
	//glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}