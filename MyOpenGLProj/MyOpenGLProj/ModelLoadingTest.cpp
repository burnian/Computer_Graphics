/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 02/17/2020, 14:48
*@Last Modify: 03/01/2020, 18:46
*@Desc: 添加第三方库分两步：
*		1.能让项目找到库文件（项目属性页->VC++目录->包含目录，库目录->分别添加include路径和lib路径）；
*		2.将.lib文件链接到项目（项目属性页->链接器->输入->附加依赖项->添加对应.lib文件）；
*		lib包含了函数所在的dll文件和文件中函数位置的信息（入口），代码由运行时加载在进程空间中的dll提供，
*		称为动态链接库dynamic link library。（这种方式更灵活，写的程序体积小，但是需要.exe和dll同时发布）
*		lib包含函数代码本身，在编译时直接将代码加入程序当中，称为静态链接库static link library。（这种方
*		式不是很灵活，因为lib被编译到.exe中，写出的程序体积大，但是只需要发布exe即可，不需要dll文件）
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



// 全局设置
const GLfloat SCR_WIDTH = 800.0f;
const GLfloat SCR_HEIGHT = 600.0f;
const GLfloat NEAR_PLANE = 0.1f;
const GLfloat FAR_PLANE = 100.0f;
// 帧数校正参数
GLfloat deltaTime = 0.0f; // Time between current frame and last frame
GLfloat lastFrame = 0.0f; // Time of last frame
// 相机
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

// 鼠标点击操作
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

// 鼠标滚轮操作
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}


GLint main() {
	// 初始化GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//opengl 主版本号设置为3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//opengl 次版本号
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

	// 鼠标设置
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 隐藏鼠标
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	// 开启线框模式
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 开启模板测试
	glEnable(GL_STENCIL_TEST);
	//@param1 sfail: action to take if the stencil test fails.
	//@param2 dpfail: action to take if the stencil test passes, but the depth test fails.
	//@param3 dppass: action to take if both the stencil and the depth test pass.
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// 开启深度测试
	glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE); //这句把 depth buffer 设置为了只读，那些通过测试的点并不能用自己的z值覆盖depth buffer
	//glDepthFunc(GL_ALWAYS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

	// 开启混合
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

	glBindVertexArray(planeVAO); // 这里的绑定很关键，会把VAO和VBO联系起来
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);

	// 指定VBO数据区
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	// 指定VAO对数据的解析方式
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
	// 因为草那张纹理根部有一点白色，所以当纹理拉伸方式为重复整张纹理时，则在草上方会有一条白线，所以就把拉伸方式改为复制纹理边缘
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GLuint windowTexture = utils::LoadTexture("../../res/texture/window.png");

	// shader
	Shader textureShader("../../res/shader/textureShader.vs", "../../res/shader/textureShader.fs");
	Shader modelShader("../../res/shader/ModelLoading.vs", "../../res/shader/ModelLoading.fs");
	Shader singleColorShader("../../res/shader/SingleColor.vs", "../../res/shader/SingleColor.fs");
	// 平行光
	modelShader.SetupDirLight();
	// 聚光灯
	modelShader.TurnOnSpotLight();

	// load models
	Model ourModel("../../res/model/nanosuit/nanosuit.obj");

	glm::mat4 model;
	// 渲染循环，绘制顺序：1.不透明物体任意顺序；2.透明物体从远到近
	while (!glfwWindowShouldClose(window)) {
		// 帧率校正
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 渲染指令
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// 输入
		ProcessInput(window);

		// 绘制不透明物体
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
		//				该值不仅是用来比较的标杆，还能用来覆盖stencil buffer
		//@param3 mask: specifies a mask that is ANDed with both the reference value and the stored stencil value before the test compares them.Initially set to all 1s.
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		// 所有待写入stencil buffer 的值都要先与该掩码相与，再把结果写入stencil buffer
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
		// 聚光灯
		modelShader.MoveSpotLight(camera.position, camera.front);

		// 绘制选中特效
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

		// 透明物体的渲染位于不透明物之后，所有透明物体都要集中起来排序，然后从远到近渲染，之所以要排序是因为深度检测，如果近处的透明物先渲染，
		// 那么当远处的物体渲染时则会因为无法通过深度检测而被直接discard，于是近处的透明物后面就不会透明显示远处的物体，变得空无一物
		// 所以我们排序从远到近渲染透明物，透明物的渲染位于不透明物之后也是这个原因。
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

		// 交换缓冲，检查并调用事件
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);

	glfwTerminate();
	return 0;
}