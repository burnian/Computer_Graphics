/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 03/24/2020, 00:51
*@Last Modify: 03/25/2020, 01:07
*@Desc: ���Ǵ�
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
const GLfloat SCR_WIDTH = 1280.0f;
const GLfloat SCR_HEIGHT = 720.0f;
const GLfloat NEAR_PLANE = 0.1f;
const GLfloat FAR_PLANE = 1000.0f;
// ֡��У������
GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame
// ���
Camera camera(glm::vec3(200.0f, 50.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f);
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
	//glEnable(GL_STENCIL_TEST);

	// �������
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// ������Ȼ���
	glEnable(GL_DEPTH_TEST);

	// uniform block object
	GLuint UBOMatrices;
	glGenBuffers(1, &UBOMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, UBOMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBOMatrices, 0, 2 * sizeof(glm::mat4)); // 0��binding pointָ��UBO��0��2��mat4���������

	// asteroid field
	const GLuint amount = 100000;
	glm::mat4 *modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime()); // initialize random seed	
	GLfloat radius = 150.0f;
	GLfloat offset = 25.0f;
	GLfloat precision = 100.0f; // precision ֵԽ��displacement ģ������ֵ����Խ�ߣ���Ϊrand ���ص���һ������
	for (GLuint i = 0; i < amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
		GLfloat displacement = (rand() % (GLint)(2 * offset * precision)) / precision - offset;// ֵ��Ϊ[-offset, offset]
		GLfloat x = sin(angle) * radius + displacement;
		displacement = (rand() % (GLint)(2 * offset * precision)) / precision - offset;
		GLfloat y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
		displacement = (rand() % (GLint)(2 * offset * precision)) / precision - offset;
		GLfloat z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: scale between 0.05 and 0.25f
		GLfloat scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		GLfloat rotAngle = rand() % 360;
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}

	Model planet = Model("../../res/model/planet/planet.obj");
	Model rock = Model("../../res/model/rock/rock.obj");

	Shader textureShader("../../res/shader/textureShader.vs", "../../res/shader/textureShader.fs");
	Shader asteroidShader("../../res/shader/asteroidField.vs", "../../res/shader/asteroidField.fs");


	GLuint instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	GLuint vec4Size = sizeof(glm::vec4);
	GLuint mat4Size = sizeof(glm::mat4);
	for (Mesh &mesh : rock.meshes) {
		glBindVertexArray(mesh.VAO);
		// vertex attributes 4ά��������4�����������ɵģ�ÿ����������4 * float
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(1 * vec4Size));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(2 * vec4Size));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(3 * vec4Size));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}


	while (!glfwWindowShouldClose(window)) {
		// ֡��У��
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		ProcessInput(window);


		// render main scene
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 viewMat = camera.GetViewMatrix();
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewMat));

		// draw planet
		textureShader.Use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		textureShader.SetMat4("model", model);
		planet.Draw(textureShader);

		// draw meteorites
		asteroidShader.Use();
		asteroidShader.SetInt("material.texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rock.texturesLoaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
		for (Mesh mesh : rock.meshes) {
			glBindVertexArray(mesh.VAO);
			glDrawElementsInstanced(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}


		// �������壬��鲢�����¼�
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &UBOMatrices);

	glfwTerminate();
	return 0;
}