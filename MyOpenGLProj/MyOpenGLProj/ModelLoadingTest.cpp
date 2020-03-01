/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 02/17/2020, 14:48
*@Last Modify: 03/01/2020, 18:46
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

#include <iostream>
#include <map>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"



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

	// �����߿�ģʽ
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// ����ģ�����
	glEnable(GL_STENCIL_TEST);
	//@param1 sfail: action to take if the stencil test fails.
	//@param2 dpfail: action to take if the stencil test passes, but the depth test fails.
	//@param3 dppass: action to take if both the stencil and the depth test pass.
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// ������Ȳ���
	glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE); //���� depth buffer ����Ϊ��ֻ������Щͨ�����Եĵ㲢�������Լ���zֵ����depth buffer
	//glDepthFunc(GL_ALWAYS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

	// �������
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	GLuint planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);

	glBindVertexArray(planeVAO); // ����İ󶨺ܹؼ������VAO��VBO��ϵ����
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);

	// ָ��VBO������
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	// ָ��VAO�����ݵĽ�����ʽ
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	GLuint floorTexture = utils::LoadTexture("../../res/texture/metal.png");

	// transparent things
	std::vector<glm::vec3> windowPos{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};

	std::vector<glm::vec3> vegetationPos{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};

	float transVertices[] {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};

	GLuint transparentVAO, transparentVBO;
	glGenVertexArrays(1, &transparentVAO);
	glGenBuffers(1, &transparentVBO);

	glBindVertexArray(transparentVAO);
	glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(transVertices), transVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);

	GLuint grassTexture = utils::LoadTexture("../../res/texture/grass.png");
	// ��Ϊ���������������һ���ɫ�����Ե��������췽ʽΪ�ظ���������ʱ�����ڲ��Ϸ�����һ�����ߣ����ԾͰ����췽ʽ��Ϊ���������Ե
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GLuint windowTexture = utils::LoadTexture("../../res/texture/window.png");

	// shader
	Shader textureShader("../../res/shader/textureShader.vs", "../../res/shader/textureShader.fs");
	Shader modelShader("../../res/shader/ModelLoading.vs", "../../res/shader/ModelLoading.fs");
	Shader singleColorShader("../../res/shader/SingleColor.vs", "../../res/shader/SingleColor.fs");
	// ƽ�й�
	modelShader.SetupDirLight();
	// �۹��
	modelShader.TurnOnSpotLight();

	// load models
	Model ourModel("../../res/model/nanosuit/nanosuit.obj");

	glm::mat4 model;
	// ��Ⱦѭ��������˳��1.��͸����������˳��2.͸�������Զ����
	while (!glfwWindowShouldClose(window)) {
		// ֡��У��
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// ��Ⱦָ��
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// ����
		ProcessInput(window);

		// ���Ʋ�͸������
		// make sure we don't update the stencil buffer while drawing the floor
		glStencilMask(0x00);

		// floor
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(planeVAO);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		textureShader.Use();
		textureShader.SetMat4("view", camera.GetViewMatrix());
		textureShader.SetMat4("projection", projection);
		model = glm::mat4(1.0f);
		textureShader.SetMat4("model", model);
		textureShader.SetInt("texture1", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// nanosuit
		//@param1 func: sets the stencil test function that determines whether a fragment passes or is discarded.
		//				This test function is applied to the stored stencil value and the glStencilFunc's ref value.
		//				Possible options are: GL_NEVER, GL_LESS, GL_LEQUAL, GL_GREATER, GL_GEQUAL, GL_EQUAL, GL_NOTEQUAL and GL_ALWAYS.
		//@param2 ref: specifies the reference value for the stencil test.The stencil buffer's content is compared to this value.
		//				��ֵ�����������Ƚϵı�ˣ�������������stencil buffer
		//@param3 mask: specifies a mask that is ANDed with both the reference value and the stored stencil value before the test compares them.Initially set to all 1s.
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		// ���д�д��stencil buffer ��ֵ��Ҫ������������룬�ٰѽ��д��stencil buffer
		glStencilMask(0xFF); // enable writing to the stencil buffer
		modelShader.Use();
		modelShader.SetVec3("viewPos", camera.position);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f));	// it's a bit too big for our scene, so scale it down
		modelShader.SetMat4("model", model);
		modelShader.SetMat4("view", camera.GetViewMatrix());
		modelShader.SetMat4("projection", projection);
		ourModel.Draw(modelShader);
		// �۹��
		modelShader.MoveSpotLight(camera.position, camera.front);

		// ����ѡ����Ч
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		singleColorShader.Use();
		GLfloat coverScale = 1.02f;
		model = glm::scale(model, glm::vec3(coverScale));	// it's a bit too big for our scene, so scale it down
		model = glm::translate(model, glm::vec3(0.0f, -0.2f, 0.0f));
		singleColorShader.SetMat4("model", model);
		singleColorShader.SetMat4("view", camera.GetViewMatrix());
		singleColorShader.SetMat4("projection", projection);
		ourModel.Draw(singleColorShader);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0x00);
		glEnable(GL_DEPTH_TEST);

		// ͸���������Ⱦλ�ڲ�͸����֮������͸�����嶼Ҫ������������Ȼ���Զ������Ⱦ��֮����Ҫ��������Ϊ��ȼ�⣬���������͸��������Ⱦ��
		// ��ô��Զ����������Ⱦʱ�����Ϊ�޷�ͨ����ȼ�����ֱ��discard�����ǽ�����͸�������Ͳ���͸����ʾԶ�������壬��ÿ���һ��
		// �������������Զ������Ⱦ͸���͸�������Ⱦλ�ڲ�͸����֮��Ҳ�����ԭ��
		std::map<GLfloat, glm::vec3> sorted;
		//for (GLuint i = 0; i < vegetationPos.size(); i++) {
		//	GLfloat distance = glm::length(camera.position - vegetationPos[i]);
		//	sorted[distance] = vegetationPos[i];
		//}
		for (GLuint i = 0; i < windowPos.size(); i++) {
			GLfloat distance = glm::length(camera.position - windowPos[i]);
			sorted[distance] = windowPos[i];
		}
		// grass
		glBindVertexArray(transparentVAO);
		//glBindTexture(GL_TEXTURE_2D, grassTexture);
		textureShader.Use();
		//for (std::map<GLfloat, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, it->second);
		//	textureShader.SetMat4("model", model);
		//	glDrawArrays(GL_TRIANGLES, 0, 6);
		//}
		// window
		glBindTexture(GL_TEXTURE_2D, windowTexture);
		for (std::map<GLfloat, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, it->second);
			textureShader.SetMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// �������壬��鲢�����¼�
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);

	glfwTerminate();
	return 0;
}