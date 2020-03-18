/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 03/13/2020, 10:35
*@Last Modify: 03/18/2020, 14:46
*@Desc: ���ӵ��������������
*		1.������Ŀ�ҵ����ļ�����Ŀ����ҳ->VC++Ŀ¼->����Ŀ¼����Ŀ¼->�ֱ�����include·����lib·������
*		2.��.lib�ļ����ӵ���Ŀ����Ŀ����ҳ->������->����->����������->���Ӷ�Ӧ.lib�ļ�����
*		lib�����˺������ڵ�dll�ļ����ļ��к���λ�õ���Ϣ����ڣ�������������ʱ�����ڽ��̿ռ��е�dll�ṩ��
*		��Ϊ��̬���ӿ�dynamic link library�������ַ�ʽ����д�ĳ������С��������Ҫ.exe��dllͬʱ������
*		lib�����������뱾�����ڱ���ʱֱ�ӽ������������У���Ϊ��̬���ӿ�static link library�������ַ�
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
const GLfloat SCR_WIDTH = 1280.0f;
const GLfloat SCR_HEIGHT = 720.0f;
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
	//glEnable(GL_STENCIL_TEST);

	// ������Ȳ���
	//glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE); //���� depth buffer ����Ϊ��ֻ������Щͨ�����Եĵ㲢�������Լ���zֵ����depth buffer
	//glDepthFunc(GL_ALWAYS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

	// �������
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// ������ü�
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	// uniform block object
	GLuint UBOMatrices;
	glGenBuffers(1, &UBOMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, UBOMatrices);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// 0��binding pointָ��UBO��0��2��mat4���������
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBOMatrices, 0, sizeof(glm::mat4));

	// screen quad
	GLfloat quadVertices[] = {
		// positions   // texCoords
		-0.2f, 1.0f,  0.0f, 1.0f,
		-0.2f, 0.6f,  0.0f, 0.0f,
		 0.2f, 0.6f,  1.0f, 0.0f,

		-0.2f, 1.0f,  0.0f, 1.0f,
		 0.2f, 0.6f,  1.0f, 0.0f,
		 0.2f, 1.0f,  1.0f, 1.0f
	};

	GLuint quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

	// ���shader ���Ƶľ��Ǹ���������������Ⱦ��һ��ƽ������
	Shader screenShader("../../res/shader/screenShader.vs", "../../res/shader/screenShader.fs");
	screenShader.Use();
	screenShader.SetInt("screenTexture", 0);
	screenShader.BindUniformBlock("Matrices", 0);

	// framebuffer configure
	GLuint FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);// GL_READ_FRAMEBUFFER, GL_DRAW_FRAMEBUFFER, ��ǰ�󶨵����ַ�ʽ��ʾǰ���ֲ������Ƕ�FBO���е�

	// generate texture
	GLuint texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//@param1 target: the framebuffer type we're targeting (draw, read or both).
	//@param2 attachment : the type of attachment we're going to attach. Note that the 0 at the end suggests we can attach more than 1 color attachment.
	//@param3 textarget : the type of the texture you want to attach.
	//@param4 texture : the actual texture to attach.
	//@param5 level : the mipmap level. We keep this at 0.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	// renderbuffer��ר���������������framebuffer�ϵģ�ֻ��д�������ٶȿ죬����texture object����ͨ�ã�
	// �����ʺϷ���Ȳ��Ժ�ģ�����������������Ҫ�������������ԣ����ܷ�renderbuffer�
	// renderbuffer��texture������framebuffer��attachment��
	GLuint RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // attach it

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// cube
	GLfloat cubeVertices[] = {
		// Back face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		// Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		// Left face
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		// Right face
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
		// Bottom face
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		// Top face
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
	};

	GLuint cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	GLuint containerTexture = utils::LoadTexture("../../res/texture/container2.png");

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

	GLuint floorFrontTexture = utils::LoadTexture("../../res/texture/metal.png");
	GLuint floorBackTexture = utils::LoadTexture("../../res/texture/marble.jpg");

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

	float transVertices[]{
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
	// ��Ϊ����������������һ���ɫ�����Ե��������췽ʽΪ�ظ���������ʱ�����ڲ��Ϸ�����һ�����ߣ����ԾͰ����췽ʽ��Ϊ����������Ե
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GLuint windowTexture = utils::LoadTexture("../../res/texture/window.png");

	// shader
	Shader textureShader("../../res/shader/textureShader.vs", "../../res/shader/textureShader.fs");
	textureShader.Use();
	textureShader.SetInt("texture1", 0);
	textureShader.SetInt("backTexture", 1);
	textureShader.BindUniformBlock("Matrices", 0);

	// load models
	Model ourModel("../../res/model/nanosuit/nanosuit.obj");

	// skybox
	GLfloat skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);

	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

	Shader skyboxShader("../../res/shader/skybox.vs", "../../res/shader/skybox.fs");
	skyboxShader.Use();
	skyboxShader.SetInt("skybox", 0);
	skyboxShader.BindUniformBlock("Matrices", 0);

	GLfloat mirrorCubeVertices[] = {
		// position           // normal
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	GLuint mirrorCubeVAO, mirrorCubeVBO;
	glGenVertexArrays(1, &mirrorCubeVAO);
	glGenBuffers(1, &mirrorCubeVBO);

	glBindVertexArray(mirrorCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mirrorCubeVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(mirrorCubeVertices), mirrorCubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	Shader mirrorCubeShader("../../res/shader/cubemap.vs", "../../res/shader/cubemap.fs");
	mirrorCubeShader.Use();
	mirrorCubeShader.SetInt("skybox", 0);
	mirrorCubeShader.BindUniformBlock("Matrices", 0);

	std::vector<std::string> faces{
		"../../res/texture/skybox/right.jpg",
		"../../res/texture/skybox/left.jpg",
		"../../res/texture/skybox/top.jpg",
		"../../res/texture/skybox/bottom.jpg",
		"../../res/texture/skybox/front.jpg",
		"../../res/texture/skybox/back.jpg",
	};
	GLuint skyboxTexture = utils::LoadCubemap(faces);


	glm::mat4 model;
	// ��Ⱦѭ��������˳��1.��͸����������˳��2.͸�������Զ����
	while (!glfwWindowShouldClose(window)) {
		// ֡��У��
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		ProcessInput(window);

		// render main scene
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ����view space����ͱ���任����
		glm::mat4 viewMat = camera.GetViewMatrix();
		camera.LookBack();
		glm::mat4 backViewMat = camera.GetViewMatrix();
		camera.LookBack();

		// ���Ʋ�͸������
		// floor
		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorFrontTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, floorBackTexture);
		textureShader.Use();
		textureShader.SetMat4("view", viewMat);
		model = glm::mat4(1.0f);
		textureShader.SetMat4("model", model);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		textureShader.SetMat4("view", backViewMat);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// cube
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, containerTexture);
		textureShader.Use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, 0.0f, -1.0f));
		textureShader.SetMat4("model", model);
		textureShader.SetMat4("view", viewMat);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		textureShader.SetMat4("view", backViewMat);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(mirrorCubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, skyboxTexture);
		mirrorCubeShader.Use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0f));
		mirrorCubeShader.SetMat4("model", model);
		mirrorCubeShader.SetMat4("view", viewMat);
		mirrorCubeShader.SetVec3("viewPos", camera.position);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		mirrorCubeShader.SetMat4("view", backViewMat);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// skybox
		// ��ͷ�ƶ���Ч����������������������Ծ�ͷ�ƶ�����ͷ����û���������ǳ��������Ե����ǰ�skybox��view�任�����е�translationȥ��
		// ֮��skybox�;�ͷ����Զ������Ծ�ֹ�ˣ��ټ��Ͻ�ֹskyboxд����Ȼ��壬����ζ��������ģ�ͱ�skybox�ڵ���Ҳ��Ȼ����ͨ��depth testing
		glDepthFunc(GL_LEQUAL);
		skyboxShader.Use();
		skyboxShader.SetMat4("view", glm::mat4(glm::mat3(viewMat))); // ȥ��ƽ�Ʊ任
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		skyboxShader.SetMat4("view", glm::mat4(glm::mat3(backViewMat)));
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

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
		// glass
		glBindVertexArray(transparentVAO);
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, grassTexture);
		textureShader.Use();
		//for (std::map<GLfloat, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, it->second);
		//	textureShader.SetMat4("model", model);
		//	glDrawArrays(GL_TRIANGLES, 0, 6);
		//}
		// red window
		glBindTexture(GL_TEXTURE_2D, windowTexture);
		for (std::map<GLfloat, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, it->second);
			textureShader.SetMat4("model", model);
			textureShader.SetMat4("view", viewMat);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			textureShader.SetMat4("view", backViewMat);
			glBindFramebuffer(GL_FRAMEBUFFER, FBO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// rear-mirror
		// һ�������Զ���FBO��������л������֮���ִ�����´���
		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer); // use the color attachment texture as the texture of the quad plane
		screenShader.Use();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glEnable(GL_DEPTH_TEST);

		// �������壬��鲢�����¼�
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteVertexArrays(1, &transparentVAO);
	glDeleteBuffers(1, &transparentVBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);

	glfwTerminate();
	return 0;
}