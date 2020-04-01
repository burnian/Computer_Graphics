/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 08/30/2019, 13:36
*@Last Modify: 03/29/2020, 16:41
*@Desc: 着色器
*********************************************************/
#pragma once
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = nullptr) {
		// 从文件路径中获取顶点/片段着色器
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// 保证ifstream对象可以抛出异常
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			// 打开文件
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			// 读取文件的缓冲内容到数据流中
			std::stringstream vShaderStream, fShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			vShaderFile.close();
			fShaderFile.close();
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			// 以下方式之所以不能使用，是因为s和t这两个string都是try这个闭包中的局部变量，出了该闭包就释放了，此时指向s和t的vv，ff这两个字符串常量指针就指向了一块随机内存
			//std::string s = vShaderStream.str();
			//std::string t = fShaderStream.str();
			//vv = s.c_str();
			//ff = t.c_str();
			if (geometryPath) {
				std::ifstream gShaderFile;
				gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
				gShaderFile.open(geometryPath);
				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		ID = glCreateProgram();
		// 顶点着色器
		const GLchar* vShaderCode = vertexCode.c_str();
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);// @para2 传递的源码字符串数量
		glCompileShader(vertexShader);
		CheckCompileErrors(vertexShader, "VERTEX");
		glAttachShader(ID, vertexShader);
		glDeleteShader(vertexShader);
		// 片元着色器
		const GLchar* fShaderCode = fragmentCode.c_str();
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);
		CheckCompileErrors(fragmentShader, "FRAGMENT");
		glAttachShader(ID, fragmentShader);
		glDeleteShader(fragmentShader);
		// 几何着色器
		if (geometryPath) {
			const GLchar* gShaderCode = geometryCode.c_str();
			GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometryShader, 1, &gShaderCode, NULL);
			glCompileShader(geometryShader);
			CheckCompileErrors(geometryShader, "GEOMETRY");
			glAttachShader(ID, geometryShader);
			glDeleteShader(geometryShader);
		}
		// 链接顶点和片段着色器到一个着色器程序中，当链接着色器至一个程序的时候，它会把每个着色器的输出链接到下个着色器的输入。当输出和输入不匹配的时候，你会得到一个连接错误。
		glLinkProgram(ID);
		CheckCompileErrors(ID, "PROGRAM");
	};

	~Shader() {};

	// 激活该shader 程序
	void Use() {
		glUseProgram(ID);
	};

	// 把shader程序中的uniform block和binding point关联起来。UB = uniform block
	//@param name shader中的uniform block名
	void BindUniformBlock(const std::string &UBName, GLuint UBbindingPoint) {
		GLuint UBIndex = glGetUniformBlockIndex(ID, UBName.c_str());
		glUniformBlockBinding(ID, UBIndex, UBbindingPoint);
	};

	// 打开平行光
	void SetupDirLight(glm::vec3 dir = glm::vec3(1.0f, 1.0f, 0.0f)) {
		Use();
		SetVec3("dirLight.direction", dir);
		SetVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
		SetVec3("dirLight.diffuse", 1.0f, 1.0f, 1.0f);
		SetVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
	}

	// 打开聚光灯
	void TurnOnSpotLight() {
		Use();
		SetFloat("spotLight.innerCos", glm::cos(glm::radians(12.5f)));
		SetFloat("spotLight.outerCos", glm::cos(glm::radians(15.0f)));
		SetVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		SetVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		SetVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		SetFloat("spotLight.constant", 1.0f); // 有效光照衰减范围50
		SetFloat("spotLight.linear", 0.09f);
		SetFloat("spotLight.quadratic", 0.032f);
	}

	void MoveSpotLight(glm::vec3 position, glm::vec3 front) {
		SetVec3("spotLight.position", position);
		SetVec3("spotLight.direction", front);
	}

	// uniform工具函数
	void SetBool(const std::string &name, GLboolean value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (GLint)value);
	};

	void SetInt(const std::string &name, GLint value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	};

	void SetFloat(const std::string &name, GLfloat value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	};

	void SetVec2(const std::string &name, const glm::vec2 &value) const {
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	};

	void SetVec2(const std::string &name, GLfloat x, GLfloat y) const {
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	};

	void SetVec3(const std::string &name, const glm::vec3 &value) const {
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	};

	void SetVec3(const std::string &name, GLfloat x, GLfloat y, GLfloat z) const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	};

	void SetVec4(const std::string &name, const glm::vec4 &value) const {
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	};

	void SetVec4(const std::string &name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const {
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	};

	void SetMat4(const std::string &name, glm::mat4 mat) const {
		//@param1 transform 这个uniform 变量的位置
		//@param2 1个矩阵
		//@param3 是否需要转置矩阵
		//@param4 GLM 和OpenGL 储存矩阵数据的方式不一样，所以用GLM的内部方法value_ptr 来对trans 进行转换
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
	};

	GLuint ID;

private:
	void CheckCompileErrors(GLuint shader, const std::string &type) {
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER::COMPILATION_FAILED of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER::LINKING_FAILED of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	};
};

