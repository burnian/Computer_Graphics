/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 08/30/2019, 17:39
*@Last Modify: 01/30/2020, 13:38
*@Desc: ...
*********************************************************/
#include <glad/glad.h> // warning: ��ȷ�����ڰ���GLFW��ͷ�ļ�֮ǰ������GLAD��ͷ�ļ���GLAD��ͷ�ļ���������ȷ��OpenGLͷ�ļ�������GL/gl.h����������Ҫ������������OpenGL��ͷ�ļ�֮ǰ����GLAD��
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "stb_image.h" // һ�����е�ͼƬ���ؿ�
#include "Shader.h"
#include "Camera.h"


// ȫ������
const GLfloat SCR_WIDTH = 800.0f;
const GLfloat SCR_HEIGHT = 600.0f;
const GLfloat NEAR_PLANE = 0.1f;
const GLfloat FAR_PLANE = 100.0f;
// ֡��У������
GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame
// ���
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::mat4 projection = glm::perspective(glm::radians(camera.fov), SCR_WIDTH / SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);
// �����������
GLfloat scale = 1.0f;
GLfloat show = 0.5f;
GLfloat speed = 0.01f;

// ��������
void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		scale += speed;
		if (scale > 10.0f)
			scale = 10.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		scale -= speed;
		if (scale < 0.001f)
			scale = 0.001f;
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)	{
		show += speed;
		if (show > 1.0f)
			show = 1.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		show -= speed;
		if (show < 0.0f)
			show = 0.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera::RIGHT, deltaTime);
}

// ����������
GLfloat lastX = 400, lastY = 300;
GLboolean isFirstMouse = true;
void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
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

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
	projection = glm::perspective(glm::radians(camera.fov), SCR_WIDTH / SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);
}


int _main() {
	// ��ʼ��GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//opengl ���汾������Ϊ3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//opengl �ΰ汾��
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//����GLFW����ʹ�õ��Ǻ���ģʽ
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// GLFW�汾��
	int major, minor, rev;
	glfwGetVersion(&major, &minor, &rev);
	std::cout << "GLFW Version: " << major << "." << minor << "." << rev << std::endl << std::endl;

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
	glfwSetFramebufferSizeCallback(window, [] (GLFWwindow* win, int w, int h) {
		glViewport(0, 0, w, h);
	});

	// ��ǰӲ���������Ķ����������ޣ�OpenGLȷ��������16������4�����Ķ������Կ���
	//GLint nrAttributes;
	//glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	//std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	// �����߿�ģʽ
	//@para1 Ӧ�õ����������ε�����ͱ��棻@para2 ���û���ģʽ����(GL_LINE) ���(GL_FILL)
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// ��shader�������ַ�������ʽд�����ÿ��GLSL����ĩβ��\n���ܺ���
	// ������Ǵ����һ����ɫ������һ����ɫ���������ݣ������ͷ�������ͽ��շ����������ͺ����ֶ�һ����ʱ��OpenGL�ͻ�������������ӵ�һ��
	// �����������һ��uniformȴ��GLSL������û�ù����������ᾲĬ�Ƴ����������������������İ汾�в����������
	// GLSL�е������������������ͣ�vecn��nάGLfloat����bvecn��nάbool����ivecn��int����uvecn��uint����dvecn��double��
	Shader shader("../../res/shader/vertexShader.vs", "../../res/shader/fragmentShader.fs");

	// �����ƶ�������
	const GLfloat triangle[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};

	const GLfloat quad1[] = {
		// ��һ��������
		0.5f, 0.5f, 0.0f, // ����
		0.5f, -0.5f, 0.0f, // ����
		-0.5f, -0.5f, 0.0f, // ����
		// �ڶ���������
		-0.5f, -0.5f, 0.0f, // ����
		0.5f, 0.5f, 0.0f, // ����
		-0.5f, 0.5f, 0.0f // ����
	};

	const GLfloat quad2[] = {
		// λ��				// ��ɫ				// ��������
		0.5f, 0.5f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,// ����
		0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f,// ����
		-0.5f, -0.5f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,// ����
		-0.5f, 0.5f, 0.0f,	1.0f, 1.0f, 0.0f,	0.0f, 1.0f // ����
	};
	const GLuint indices[] = {
		0, 1, 3, // ��һ��������
		1, 2, 3 // �ڶ���������
	};

	// һ������������ж���
	const GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	glm::vec3 cubePos[] = {
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

	// ���ɲ����������VAO��VBO�����߶���Ҫ���ٴ洢�ռ��
	// vertexArrayObject��VAO�б������ͨ��glVertexAttribPointer���õĶ����������ã���glBindVertexArray��VAO�Ժ󣬾ͻ�ͨ��glVertexAttribPointer�����붥�����Թ�����VBO
	// VAOʹ���ڲ�ͬ�������ݺ���������֮���л���÷ǳ��򵥣�ֻ��Ҫ�󶨲�ͬ��VAO�����ˡ�
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	// vertexBufferObject��VBO�л��������еĶ���������Ϣ
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW); // GL_STATIC_DRAW��ʾ�����ζ������ݲ��ᱻ�ı�
	//glBufferData(GL_ARRAY_BUFFER, sizeof(quad1), quad1, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(quad2), quad2, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// elementBufferObject
	//GLuint EBO;
	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//@para4 ָ��������ϣ���Կ���ι�����������ݡ�GL_STATIC_DRAW ���ݲ���򼸺�����ı䣬GL_DYNAMIC_DRAW ��GL_STREAM_DRAW ��ȷ���Կ������ݷ����ܹ�����д����ڴ沿��
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// ���ö�������ָ�룬�ú�������ָ��������ɫ�����������ݵĽ�����ʽ
	//@para1 ������ɫ����λ��ֵ
	//@para2 ����������һ����Ԫ��
	//@para4 �Ƿ�ϣ�����ݱ�׼����ӳ�䵽0~1��
	//@para5 ������������������֮��ļ������������֪����������ǽ������еģ���������������֮��û�п�϶������Ҳ��������Ϊ0����OpenGL�������岽���Ƕ��٣�ֻ�е���ֵ�ǽ�������ʱ�ſ��ã�
	//@para6 ���ݵ�ƫ��������������������Ŀ�ͷ��
	// λ������
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);// ��Ϊ������ɫ��Դ����layout(location = 0)�����Ե�һ��������0
	//glEnableVertexAttribArray(0);
	// ��ɫ����
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);
	// ��������
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(2);

	// λ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);// ��Ϊ������ɫ��Դ����layout(location = 0)�����Ե�һ��������0
	glEnableVertexAttribArray(0);
	// ��������
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);


	// ���VAO��VBO��EBO
	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	//glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// ����OpenGL�Ļ��ƹ����ǰ�VAO1��VBO1�����VAO1��VBO1����VAO2��VBO2�����VAO2��VBO2������  ��������˭�Ͱ󶨶�Ӧ��VAO

	// ��������
	stbi_set_flip_vertically_on_load(true);
	// ����1
	GLuint texture;
	//@para1 �������������
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Ϊ����������û��Ʒ�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 0.0f, 0.34f, 0.57f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// Ϊ����������ù��˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);// ����Сģ��֮��ģ������Ĳ�����ʽ��ͬʱҲ�����ڴ����ö༶��Զ����Ĺ��ɲ���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);// ���Ŵ�ģ��֮��ģ������Ĳ�����ʽ
	// ���ز���������
	GLint width, height, channels;
	// FileSystem::getPath("resources/textures/container.jpg").c_str();
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform;
	unsigned char* data = stbi_load("../../res/texture/container.jpg", &width, &height, &channels, 0);
	if (data) {
		//@para1 ����ΪGL_TEXTURE_2D��ζ�Ż������뵱ǰ�󶨵����������ͬһ��Ŀ���ϵ������κΰ󶨵�GL_TEXTURE_1D��GL_TEXTURE_3D���������ܵ�Ӱ�죩
		//@para2 �༶��Զ����ļ���0�ǻ�������
		//@para3 �������ʽ�����ǵ�ͼ��ֻ��RGBֵ���������Ҳ��������ΪRGBֵ��
		//@para4 ���յ������Ⱥ͸߶�
		//@para5 ����0����ʷ�������⣩
		//@para6 Դͼ�ĸ�ʽ
		//@para7 Դͼ����������
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	// ����2
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = stbi_load("../../res/texture/awesomeface.png", &width, &height, &channels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	shader.Use(); // �������ȼ�����ɫ��
	shader.SetInt("texture1", 0);
	shader.SetInt("texture2", 1);

	// ��2DͼƬƽ����3D�ռ��еı任����
	//glm::mat4 view = glm::mat4(1.0f);
	//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); // we're translating the scene in the reverse direction of where we want to move
	//shader.SetMat4("view", view);

	// ������Ȳ���
	glEnable(GL_DEPTH_TEST);

	// �������
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // �������
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	// ��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {
		// ����
		ProcessInput(window);

		// ��Ⱦָ��
		glClearColor(0.0f, 0.34f, 0.57f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ֡��У��
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// ����
		shader.Use();
		//GLfloat timeValue = glfwGetTime();
		//GLfloat filterValue = sin(2 * timeValue) * 0.5f;
		//shader.SetFloat("xOffset", filterValue);
		shader.SetFloat("show", show);
		shader.SetFloat("scale", scale);

		shader.SetMat4("view", camera.GetViewMatrix());
		shader.SetMat4("projection", projection);

		// �任����
		//glm::mat4 trans = glm::mat4();
		//trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
		//trans = glm::rotate(trans, (GLfloat)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		//trans = glm::scale(trans, glm::vec3(sin((GLfloat)glfwGetTime())));
		//for (int i = 0; i < 4; i++) {
		//	for (int j = 0; j < 4; j++) {
		//		std::cout << trans[j][i];
		//	}
		//	std::cout << std::endl;
		//}
		//shader.SetMat4("transform", trans);
		//glm::mat4 model = glm::mat4(1.0f);
		//model = glm::rotate(model, (GLfloat)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
		//shader.SetMat4("model", model);

		for (unsigned int i = 0; i < 10; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePos[i]);
			model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
			if (i % 3 == 0) {
				model = glm::rotate(model, (GLfloat)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
			}
			shader.SetMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// ��VAO
		glBindVertexArray(VAO);
		// ���еĻ��Ʋ�������ͨ��VAO�ҵ���ӦVBO�����Ķ���������Ϣ�����л���
		//glDrawArrays(GL_TRIANGLES, 0, 3);// ����������
		//glDrawArrays(GL_TRIANGLES, 0, 6);// �����ı���

		//@param3 ����������
		//@param4 EBO�е�ƫ����
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// �������壬��鲢�����¼�
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

