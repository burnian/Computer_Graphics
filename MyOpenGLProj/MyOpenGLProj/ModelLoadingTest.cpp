/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 02/17/2020, 14:48
*@Last Modify: 02/18/2020, 22:18
*@Desc: ��ӵ��������������
*		1.������Ŀ�ҵ����ļ�����Ŀ����ҳ->VC++Ŀ¼->����Ŀ¼����Ŀ¼->�ֱ����include·����lib·������
*		2.��.lib�ļ����ӵ���Ŀ����Ŀ����ҳ->������->����->����������->��Ӷ�Ӧ.lib�ļ�����
*		lib�����˺������ڵ�dll�ļ����ļ��к���λ�õ���Ϣ����ڣ�������������ʱ�����ڽ��̿ռ��е�dll�ṩ��
*		��Ϊ��̬���ӿ�dynamic link library�������ַ�ʽ����д�ĳ������С��������Ҫ.exe��dllͬʱ������
*		lib�����������뱾���ڱ���ʱֱ�ӽ������������У���Ϊ��̬���ӿ�static link library�������ַ�
*		ʽ���Ǻ�����Ϊlib�����뵽.exe�У�д���ĳ�������󣬵���ֻ��Ҫ����exe���ɣ�����Ҫdll�ļ���
*********************************************************/
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <iostream>


// ȫ������
const GLfloat SCR_WIDTH = 800.0f;
const GLfloat SCR_HEIGHT = 600.0f;
const GLfloat NEAR_PLANE = 0.1f;
const GLfloat FAR_PLANE = 100.0f;
// ֡��У������
GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame
// ���
Camera camera(glm::vec3(-2.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f);
glm::mat4 projection = glm::perspective(glm::radians(camera.fov), SCR_WIDTH / SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);


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

// ���������
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

// �����ֲ���
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}


GLint main() {
	// ��ʼ��GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//opengl ���汾������Ϊ3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//opengl �ΰ汾��
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//����GLFW����ʹ�õ��Ǻ���ģʽ
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// ��������
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "burnian", nullptr, nullptr); // ������������Ƿ�ʹ��ȫ��ģʽ�����������ĵĴ���
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// ��ʼ��GLAD
	//warning��GLAD�ĳ�ʼ�������ڴ�������֮������֮ǰ����ʼ����ʧ��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD." << std::endl;
		return -1;
	}

	// ���ڴ�С�ı�ص�
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, GLint w, GLint h) {
		glViewport(0, 0, w, h);
	});

	// �������
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // �������
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	// ������Ȳ���
	glEnable(GL_DEPTH_TEST);
	// glDepthMask(GL_FALSE); //���� depth buffer ����Ϊ��ֻ������Щͨ�����Եĵ㲢�������Լ���zֵ����depth buffer
	//glDepthFunc(GL_ALWAYS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

	// �����߿�ģʽ
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// floor
	GLfloat planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};

	// ָ��������
	GLuint planeVBO;
	glGenBuffers(1, &planeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

	// plane VAO
	GLuint planeVAO;
	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO); // ����İ󶨺ܹؼ������VAO��VBO��ϵ����
	// ָ��VAO�����ݵĽ�����ʽ
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	GLuint floorTexture = utils::LoadTexture("../../res/texture/metal.png");

	// shader
	Shader textureShader("../../res/shader/textureShader.vs", "../../res/shader/textureShader.fs");
	Shader modelShader("../../res/shader/ModelLoading.vs", "../../res/shader/ModelLoading.fs");
	// ƽ�й�
	modelShader.SetupDirLight();
	// �۹��
	modelShader.TurnOnSpotLight();

	// load models
	Model ourModel("../../res/model/nanosuit/nanosuit.obj");

	// ��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {
		// ����
		ProcessInput(window);

		// ��Ⱦָ��
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ֡��У��
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// ģ��
		modelShader.Use();
		modelShader.SetVec3("viewPos", camera.position);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
		modelShader.SetMat4("model", model);
		modelShader.SetMat4("view", camera.GetViewMatrix());
		modelShader.SetMat4("projection", projection);
		ourModel.Draw(modelShader);
		// �۹��
		modelShader.MoveSpotLight(camera.position, camera.front);

		// floor
		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture);

		textureShader.Use();
		textureShader.SetMat4("view", camera.GetViewMatrix());
		textureShader.SetMat4("projection", projection);
		textureShader.SetMat4("model", glm::mat4(1.0f));
		textureShader.SetInt("texture1", 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		// �������壬��鲢�����¼�
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);

	glfwTerminate();
	return 0;
}