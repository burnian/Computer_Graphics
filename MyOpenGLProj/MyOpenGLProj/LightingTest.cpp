/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 01/30/2020, 13:39
*@Last Modify: 04/02/2020, 01:48
*@Desc: 光照测试
*********************************************************/
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils.h"
#include "Shader.h"
#include "Camera.h"

#include <iostream>


//--------------------------
// 全局设置
const GLfloat SCR_WIDTH = 1280.0f;
const GLfloat SCR_HEIGHT = 720.0f;
const GLfloat NEAR_PLANE = 0.1f;
const GLfloat FAR_PLANE = 100.0f;
// 帧数校正参数
GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame
// 相机
Camera camera(glm::vec3(-2.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f);
glm::mat4 projection = glm::perspective(glm::radians(camera.fov), SCR_WIDTH / SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);
// 灯光
bool spotLightOn = true;
bool isSpotBtnReleased = true;

//--------------------------
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
	// 聚光灯开关
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && isSpotBtnReleased) {
		spotLightOn = !spotLightOn;
		isSpotBtnReleased = false;
	}
	else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE && !isSpotBtnReleased) {
		isSpotBtnReleased = true;
	}
}

// 鼠标操作参数
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


//--------------------------
GLint main() {
	// 初始化GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);// opengl 主版本号设置为3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);// opengl 次版本号
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//告诉GLFW我们使用的是核心模式
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "burnian", nullptr, nullptr); // 最后俩参数：是否使用全屏模式，共享上下文的窗口
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// 初始化GLAD
	//warning：GLAD的初始化必须在创建窗口之后，在这之前，初始化会失败
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD." << std::endl;
		return -1;
	}

	// 窗口大小改变回调
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, GLint w, GLint h) {
		glViewport(0, 0, w, h);
	});

	// 鼠标事件绑定
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hide mouse
	glfwSetCursorPosCallback(window, MouseCallback);
	// glfwSetScrollCallback(window, ScrollCallback);

	//--------------------------
	// 开启深度测试
	glEnable(GL_DEPTH_TEST);

	// 开启线框模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 开启gamma correction
	//glEnable(GL_FRAMEBUFFER_SRGB);

	//--------------------------
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	GLuint containerTexture = utils::LoadTexture("../../res/texture/container2.png");
	GLuint containerSpecTexture = utils::LoadTexture("../../res/texture/container2_specular.png");
	GLuint woodTexture = utils::LoadTexture("../../res/texture/wood.png");

	//--------------------------
	Shader depthMapShader("../../res/shader/depthMap.vs", "../../res/shader/depthMap.fs");
	//Shader depthMapDebugShader("../../res/shader/depthMapDebug.vs", "../../res/shader/depthMapDebug.fs");

	Shader BPSShader("../../res/shader/BlinnPhongShadow.vs", "../../res/shader/BlinnPhongShadow.fs");
	//BPSShader.TurnOnSpotLight(); // 聚光灯
	// 点光源
	//for (GLint i = 0; i < 4; i++) {
	//	BPSShader.SetVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
	//	BPSShader.SetVec3("pointLights[" + std::to_string(i) + "].ambient", 0.05f, 0.05f, 0.05f);
	//	BPSShader.SetVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.8f, 0.8f, 0.8f);
	//	BPSShader.SetVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
	//	BPSShader.SetFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f); // 有效光照衰减范围50
	//	BPSShader.SetFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
	//	BPSShader.SetFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
	//}

	// lamp
	Shader lampShader("../../res/shader/LampShader.vs", "../../res/shader/LampShader.fs");
	lampShader.Use();
	lampShader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);

	//--------------------------
	// 灯光阴影深度纹理
	const GLfloat SHADOW_WIDTH = 1024.0f, SHADOW_HEIGHT = 1024.0f;
	const GLfloat SHADOW_NEAR = 1.0f, SHADOW_FAR = 7.5f;
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);// 因为只在乎深度值，所以纹理存储类型设为 GL_DEPTH_COMPONENT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// directional light space 变换矩阵
	glm::vec3 dirLightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, SHADOW_NEAR, SHADOW_FAR);// 平行光使用正交投影来生成深度阴影贴图
	//glm::mat4 lightProjection = glm::perspective(glm::radians(45.0f), SHADOW_WIDTH / SHADOW_HEIGHT, SHADOW_NEAR, SHADOW_FAR);// 非平行光使用透视投影来生成深度阴影贴图
	glm::mat4 lightView = glm::lookAt(dirLightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 dirLightSpaceMat = lightProjection * lightView;

	//--------------------------
	// render loop
	glm::mat4 model;
	while (!glfwWindowShouldClose(window)) {
		// 输入
		ProcessInput(window);

		// 帧率校正
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//--------------------------
		// 渲染指令
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glm::mat4 viewMat = camera.GetViewMatrix();

		// 1. render to depth map
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glClear(GL_DEPTH_BUFFER_BIT);

		depthMapShader.Use();
		depthMapShader.SetMat4("lightSpaceMatrix", dirLightSpaceMat);
		// floor
		model = glm::mat4(1.0f);
		depthMapShader.SetMat4("model", model);
		utils::RenderPlane();
		// cubes
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		depthMapShader.SetMat4("model", model);
		utils::RenderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
		model = glm::scale(model, glm::vec3(0.5f));
		depthMapShader.SetMat4("model", model);
		utils::RenderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		model = glm::scale(model, glm::vec3(0.25f));
		depthMapShader.SetMat4("model", model);
		utils::RenderCube();

		// 2. render scene as normal using the generated depth/shadow map  
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		BPSShader.Use();
		BPSShader.SetupDirLight(glm::vec3(0.0f) - dirLightPos);
		BPSShader.SetMat4("lightSpaceMatrix", dirLightSpaceMat);
		BPSShader.SetVec3("viewPos", camera.position);
		BPSShader.SetMat4("projection", projection);
		BPSShader.SetMat4("view", viewMat);
		// floor
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		BPSShader.SetInt("material.diffuse", 0);
		BPSShader.SetInt("material.specular", 0);
		BPSShader.SetFloat("material.shininess", 32.0f);
		BPSShader.SetInt("depthMap", 1);
		model = glm::mat4(1.0f);
		BPSShader.SetMat4("model", model);
		utils::RenderPlane();
		// cubes
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, containerTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, containerSpecTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		BPSShader.SetInt("material.diffuse", 0);
		BPSShader.SetInt("material.specular", 1);
		BPSShader.SetFloat("material.shininess", 64.0f);
		BPSShader.SetInt("depthMap", 2);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
		model = glm::scale(model, glm::vec3(0.5f));
		BPSShader.SetMat4("model", model);
		utils::RenderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
		model = glm::scale(model, glm::vec3(0.5f));
		BPSShader.SetMat4("model", model);
		utils::RenderCube();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
		model = glm::scale(model, glm::vec3(0.25));
		BPSShader.SetMat4("model", model);
		utils::RenderCube();



		//depthMapDebugShader.Use();
		////depthMapDebugShader.SetFloat("near_plane", near_plane);
		////depthMapDebugShader.SetFloat("far_plane", far_plane);
		//depthMapDebugShader.SetInt("depthMap", 0);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, depthMap);
		//utils::RenderQuad();


		// 灯光绕定点旋转
		//double sita = glm::radians(sin(currentFrame) * 180);
		//lightPos.x = cos(sita);
		//lightPos.z = sin(sita);
		// 灯光变色
		//glm::vec3 lightColor;
		//lightColor.x = fmax(sin(currentFrame * 2.0f), 0.0f);
		//lightColor.y = fmax(sin(currentFrame * 0.7f), 0.0f);
		//lightColor.z = fmax(sin(currentFrame * 1.3f), 0.0f);
		//glm::vec3 diffuseColor = lightColor * glm::vec3(0.9f);
		//glm::vec3 ambientColor = diffuseColor * glm::vec3(0.7f);

		//--------------------------
		// cube
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, containerTexture);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, containerSpecTexture);
		//glBindVertexArray(cubeVAO);
		//objectShader.Use();
		//objectShader.SetInt("spotLightOn", spotLightOn);
		//objectShader.SetVec3("viewPos", camera.position);
		//objectShader.SetMat4("view", viewMat);
		//objectShader.SetVec3("spotLight.position", camera.position);
		//objectShader.SetVec3("spotLight.direction", camera.GetFront());
		//for (GLint i = 0; i < 10; i++) {
		//	glm::mat4 model = glm::mat4(1.0f);
		//	model = glm::translate(model, cubePositions[i]);
		//	model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
		//	objectShader.SetMat4("model", model);
		//	glDrawArrays(GL_TRIANGLES, 0, 36);
		//}

		// lamp
		//lightingShader.Use();
		//lightingShader.SetMat4("view", viewMat);
		//glBindVertexArray(lightVAO);
		//for (GLint i = 0; i < 4; i++) {
		//	glm::mat4 model = glm::mat4(1.0f);
		//	model = glm::translate(model, pointLightPositions[i]);
		//	model = glm::scale(model, glm::vec3(0.2f));
		//	lightingShader.SetMat4("model", model);
		//	glDrawArrays(GL_TRIANGLES, 0, 36);
		//}


		//--------------------------
		// 交换缓冲，检查并调用事件
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//glDeleteVertexArrays(1, &cubeVAO);
	//glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}