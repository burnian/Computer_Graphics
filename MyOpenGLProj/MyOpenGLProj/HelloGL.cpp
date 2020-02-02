/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 08/30/2019, 17:39
*@Last Modify: 01/30/2020, 13:38
*@Desc: ...
*********************************************************/
#include <glad/glad.h> // warning: 请确认是在包含GLFW的头文件之前包含了GLAD的头文件。GLAD的头文件包含了正确的OpenGL头文件（例如GL/gl.h），所以需要在其它依赖于OpenGL的头文件之前包含GLAD。
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "stb_image.h" // 一个流行的图片加载库
#include "Shader.h"
#include "Camera.h"


// 全局设置
const GLfloat SCR_WIDTH = 800.0f;
const GLfloat SCR_HEIGHT = 600.0f;
const GLfloat NEAR_PLANE = 0.1f;
const GLfloat FAR_PLANE = 100.0f;
// 帧数校正参数
GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame
// 相机
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::mat4 projection = glm::perspective(glm::radians(camera.fov), SCR_WIDTH / SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);
// 纹理操作参数
GLfloat scale = 1.0f;
GLfloat show = 0.5f;
GLfloat speed = 0.01f;

// 处理输入
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

// 鼠标操作参数
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
	// 初始化GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//opengl 主版本号设置为3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//opengl 次版本号
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//告诉GLFW我们使用的是核心模式
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// GLFW版本号
	int major, minor, rev;
	glfwGetVersion(&major, &minor, &rev);
	std::cout << "GLFW Version: " << major << "." << minor << "." << rev << std::endl << std::endl;

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
	glfwSetFramebufferSizeCallback(window, [] (GLFWwindow* win, int w, int h) {
		glViewport(0, 0, w, h);
	});

	// 当前硬件能声明的顶点属性上限，OpenGL确保至少有16个包含4分量的顶点属性可用
	//GLint nrAttributes;
	//glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	//std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	// 开启线框模式
	//@para1 应用到所有三角形的正面和背面；@para2 设置绘制模式，线(GL_LINE) 填充(GL_FILL)
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 若shader代码用字符串的形式写在这里，每行GLSL代码末尾的\n不能忽略
	// 如果我们打算从一个着色器向另一个着色器发送数据，当发送方的输出和接收方的输入类型和名字都一样的时候，OpenGL就会把两个变量链接到一起
	// 如果你声明了一个uniform却在GLSL代码中没用过，编译器会静默移除这个变量，导致最后编译出的版本中并不会包含它
	// GLSL中的向量可以有以下类型：vecn（n维GLfloat），bvecn（n维bool），ivecn（int），uvecn（uint），dvecn（double）
	Shader shader("../../res/shader/vertexShader.vs", "../../res/shader/fragmentShader.fs");

	// 待绘制顶点数据
	const GLfloat triangle[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};

	const GLfloat quad1[] = {
		// 第一个三角形
		0.5f, 0.5f, 0.0f, // 右上
		0.5f, -0.5f, 0.0f, // 右下
		-0.5f, -0.5f, 0.0f, // 左下
		// 第二个三角形
		-0.5f, -0.5f, 0.0f, // 左下
		0.5f, 0.5f, 0.0f, // 右上
		-0.5f, 0.5f, 0.0f // 左上
	};

	const GLfloat quad2[] = {
		// 位置				// 颜色				// 纹理坐标
		0.5f, 0.5f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,// 右上
		0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f,// 右下
		-0.5f, -0.5f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,// 左下
		-0.5f, 0.5f, 0.0f,	1.0f, 1.0f, 0.0f,	0.0f, 1.0f // 左上
	};
	const GLuint indices[] = {
		0, 1, 3, // 第一个三角形
		1, 2, 3 // 第二个三角形
	};

	// 一个正方体的所有顶点
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

	// 生成并绑定立方体的VAO和VBO，二者都是要开辟存储空间的
	// vertexArrayObject，VAO中保存的是通过glVertexAttribPointer设置的顶点属性配置，当glBindVertexArray绑定VAO以后，就会通过glVertexAttribPointer调用与顶点属性关联的VBO
	// VAO使得在不同顶点数据和属性配置之间切换变得非常简单，只需要绑定不同的VAO就行了。
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	// vertexBufferObject，VBO中缓冲了所有的顶点属性信息
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW); // GL_STATIC_DRAW表示三角形顶点数据不会被改变
	//glBufferData(GL_ARRAY_BUFFER, sizeof(quad1), quad1, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(quad2), quad2, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// elementBufferObject
	//GLuint EBO;
	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//@para4 指定了我们希望显卡如何管理给定的数据。GL_STATIC_DRAW 数据不会或几乎不会改变，GL_DYNAMIC_DRAW 和GL_STREAM_DRAW 能确保显卡把数据放在能够高速写入的内存部分
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 设置顶点属性指针，该函数用来指定顶点着色器中输入数据的解析方式
	//@para1 顶点着色器的位置值
	//@para2 顶点属性是一个三元数
	//@para4 是否希望数据标准化（映射到0~1）
	//@para5 步长，连续顶点属性之间的间隔，由于我们知道这个数组是紧密排列的（在两个顶点属性之间没有空隙）我们也可以设置为0来让OpenGL决定具体步长是多少（只有当数值是紧密排列时才可用）
	//@para6 数据的偏移量（这里数据在数组的开头）
	// 位置属性
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);// 因为顶点着色器源码中layout(location = 0)，所以第一个参数填0
	//glEnableVertexAttribArray(0);
	// 颜色属性
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);
	// 纹理属性
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(2);

	// 位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);// 因为顶点着色器源码中layout(location = 0)，所以第一个参数填0
	glEnableVertexAttribArray(0);
	// 纹理属性
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);


	// 解绑VAO、VBO和EBO
	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	//glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// 所以OpenGL的绘制过程是绑定VAO1，VBO1，解绑VAO1，VBO1，绑定VAO2，VBO2，解绑VAO2，VBO2，……  最后想绘制谁就绑定对应的VAO

	// 加载纹理
	stbi_set_flip_vertically_on_load(true);
	// 纹理1
	GLuint texture;
	//@para1 生成纹理的数量
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// 为纹理对象设置环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 0.0f, 0.34f, 0.57f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// 为纹理对象设置过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);// 当缩小模型之后，模型纹理的采样方式，同时也可以在此设置多级渐远纹理的过渡采样
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);// 当放大模型之后，模型纹理的采样方式
	// 加载并生成纹理
	GLint width, height, channels;
	// FileSystem::getPath("resources/textures/container.jpg").c_str();
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform;
	unsigned char* data = stbi_load("../../res/texture/container.jpg", &width, &height, &channels, 0);
	if (data) {
		//@para1 设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理（任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）
		//@para2 多级渐远纹理的级别，0是基本级别
		//@para3 纹理储存格式（我们的图像只有RGB值，因此我们也把纹理储存为RGB值）
		//@para4 最终的纹理宽度和高度
		//@para5 总是0（历史遗留问题）
		//@para6 源图的格式
		//@para7 源图的数据类型
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	// 纹理2
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

	shader.Use(); // 别忘记先激活着色器
	shader.SetInt("texture1", 0);
	shader.SetInt("texture2", 1);

	// 把2D图片平铺在3D空间中的变换矩阵
	//glm::mat4 view = glm::mat4(1.0f);
	//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); // we're translating the scene in the reverse direction of where we want to move
	//shader.SetMat4("view", view);

	// 开启深度测试
	glEnable(GL_DEPTH_TEST);

	// 鼠标设置
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 隐藏鼠标
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	// 渲染循环
	while (!glfwWindowShouldClose(window)) {
		// 输入
		ProcessInput(window);

		// 渲染指令
		glClearColor(0.0f, 0.34f, 0.57f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 帧率校正
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 更新
		shader.Use();
		//GLfloat timeValue = glfwGetTime();
		//GLfloat filterValue = sin(2 * timeValue) * 0.5f;
		//shader.SetFloat("xOffset", filterValue);
		shader.SetFloat("show", show);
		shader.SetFloat("scale", scale);

		shader.SetMat4("view", camera.GetViewMatrix());
		shader.SetMat4("projection", projection);

		// 变换绘制
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

		// 绑定VAO
		glBindVertexArray(VAO);
		// 所有的绘制操作都会通过VAO找到对应VBO关联的顶点属性信息并进行绘制
		//glDrawArrays(GL_TRIANGLES, 0, 3);// 绘制三角形
		//glDrawArrays(GL_TRIANGLES, 0, 6);// 绘制四边形

		//@param3 索引的类型
		//@param4 EBO中的偏移量
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// 交换缓冲，检查并调用事件
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

