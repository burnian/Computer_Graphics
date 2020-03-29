/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 01/30/2020, 13:39
*@Last Modify: 03/29/2020, 16:41
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
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
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

	//--------------------------
	// 开启深度测试
	glEnable(GL_DEPTH_TEST);

	// 开启线框模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//--------------------------
	// uniform block object
	utils::ConfigureUBO(projection);

	//--------------------------
	// plane
	float planeVertices[] = {
		// positions            // normals         // texcoords
		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
		 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
	};
	GLuint planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glBindVertexArray(0);

	GLuint planeTexture = utils::LoadTexture("../../res/texture/wood.png");

	//--------------------------
	// randomly placed containers
	const GLfloat vertices[] = {
		// positions          // normals           // texture coords
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

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	GLuint containerTexture = utils::LoadTexture("../../res/texture/container2.png");
	GLuint specularTexture = utils::LoadTexture("../../res/texture/container2_specular.png");
	//glActiveTexture(GL_TEXTURE2);
	//LoadTexture("../../res/texture/matrix.jpg"); // 自发光贴图

	//--------------------------
	Shader blinnPhongShader("../../res/shader/BlinnPhong.vs", "../../res/shader/BlinnPhong.fs");
	blinnPhongShader.Use();
	blinnPhongShader.SetInt("material.diffuse", 0);
	blinnPhongShader.SetInt("material.specular", 0);
	blinnPhongShader.SetFloat("material.shininess", 32.0f);
	blinnPhongShader.SetupDirLight(); // 平行光
	blinnPhongShader.TurnOnSpotLight(); // 聚光灯
	// 点光源
	for (GLint i = 0; i < 4; i++) {
		blinnPhongShader.SetVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
		blinnPhongShader.SetVec3("pointLights[" + std::to_string(i) + "].ambient", 0.05f, 0.05f, 0.05f);
		blinnPhongShader.SetVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.8f, 0.8f, 0.8f);
		blinnPhongShader.SetVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
		blinnPhongShader.SetFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f); // 有效光照衰减范围50
		blinnPhongShader.SetFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
		blinnPhongShader.SetFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
	}

	// shader
	Shader objectShader("../../res/shader/Phong.vs", "../../res/shader/Phong.fs");
	objectShader.Use();
	objectShader.SetInt("material.diffuse", 0);
	objectShader.SetInt("material.specular", 1);
	//objectShader.SetInt("material.emission", 2); // 自发光贴图
	objectShader.SetFloat("material.shininess", 32.0f);
	objectShader.SetupDirLight(); // 平行光
	objectShader.TurnOnSpotLight(); // 聚光灯
	// 点光源
	for (int i = 0; i < 4; i++) {
		objectShader.SetVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
		objectShader.SetVec3("pointLights[" + std::to_string(i) + "].ambient", 0.05f, 0.05f, 0.05f);
		objectShader.SetVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.8f, 0.8f, 0.8f);
		objectShader.SetVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
		objectShader.SetFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f); // 有效光照衰减范围50
		objectShader.SetFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
		objectShader.SetFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
	}

	//
	Shader lightingShader("../../res/shader/LampShader.vs", "../../res/shader/LampShader.fs");
	lightingShader.Use();
	lightingShader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);

	//--------------------------
	// mouse settings
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hide mouse
	glfwSetCursorPosCallback(window, MouseCallback);
	// glfwSetScrollCallback(window, ScrollCallback);

	//--------------------------
	// render loop
	glm::mat4 model;
	while (!glfwWindowShouldClose(window)) {
		// 输入
		ProcessInput(window);

		// 渲染指令
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 帧率校正
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//--------------------------
		glm::mat4 viewMat = camera.GetViewMatrix();

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
		// plane
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, planeTexture);
		glBindVertexArray(planeVAO);

		blinnPhongShader.Use();// 左边
		blinnPhongShader.SetInt("spotLightOn", spotLightOn);
		blinnPhongShader.SetVec3("viewPos", camera.position);
		blinnPhongShader.SetMat4("view", viewMat);
		blinnPhongShader.SetVec3("spotLight.position", camera.position);
		blinnPhongShader.SetVec3("spotLight.direction", camera.GetFront());
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -10.0f, -10.0f));
		blinnPhongShader.SetMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		objectShader.Use();// 右边
		objectShader.SetInt("spotLightOn", spotLightOn);
		objectShader.SetVec3("viewPos", camera.position);
		objectShader.SetMat4("view", viewMat);
		objectShader.SetVec3("spotLight.position", camera.position);
		objectShader.SetVec3("spotLight.direction", camera.GetFront());
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -10.0f, 10.0f));
		objectShader.SetMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//--------------------------
		// cube
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, containerTexture);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, specularTexture);
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

		//--------------------------
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

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}