/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 08/30/2019, 13:36
*@Last Modify: 02/19/2020, 11:49
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
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
		// 从文件路径中获取顶点/片段着色器
		std::string vertexCode;
		std::string fragmentCode;
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
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();
		// 顶点着色器
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);// @para2 传递的源码字符串数量
		glCompileShader(vertexShader);
		CheckCompileErrors(vertexShader, "VERTEX");
		// 片段着色器
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);
		CheckCompileErrors(fragmentShader, "FRAGMENT");
		// 链接顶点和片段着色器到一个着色器程序中，当链接着色器至一个程序的时候，它会把每个着色器的输出链接到下个着色器的输入。当输出和输入不匹配的时候，你会得到一个连接错误。
		ID = glCreateProgram();
		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);
		glLinkProgram(ID);
		CheckCompileErrors(ID, "PROGRAM");
		// 删除着色器对象
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	};

	~Shader() {};

	void Use() {
		glUseProgram(ID);
	};

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

	void SetVec3(const std::string &name, const glm::vec3 &value) const {
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void SetVec3(const std::string &name, GLfloat x, GLfloat y, GLfloat z) const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	};

	void SetVec4(const std::string &name, const glm::vec4 &value) const {
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void SetVec4(const std::string &name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const {
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}

	void SetMat4(const std::string &name, glm::mat4 mat) const {
		//@param1 transform 这个uniform 变量的位置
		//@param2 1个矩阵
		//@param3 是否需要转置矩阵
		//@param4 GLM 和OpenGL 储存矩阵数据的方式不一样，所以用GLM的内部方法value_ptr 来对trans 进行转换
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
	};

	GLuint ID;

private:
	void CheckCompileErrors(GLuint shader, std::string type) {
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

