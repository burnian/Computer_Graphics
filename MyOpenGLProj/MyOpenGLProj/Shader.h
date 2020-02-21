/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 08/30/2019, 13:36
*@Last Modify: 02/19/2020, 11:49
*@Desc: ��ɫ��
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
		// ���ļ�·���л�ȡ����/Ƭ����ɫ��
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// ��֤ifstream��������׳��쳣
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			// ���ļ�
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			// ��ȡ�ļ��Ļ������ݵ���������
			std::stringstream vShaderStream, fShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			vShaderFile.close();
			fShaderFile.close();
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			// ���·�ʽ֮���Բ���ʹ�ã�����Ϊs��t������string����try����հ��еľֲ����������˸ñհ����ͷ��ˣ���ʱָ��s��t��vv��ff�������ַ�������ָ���ָ����һ������ڴ�
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
		// ������ɫ��
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);// @para2 ���ݵ�Դ���ַ�������
		glCompileShader(vertexShader);
		CheckCompileErrors(vertexShader, "VERTEX");
		// Ƭ����ɫ��
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);
		CheckCompileErrors(fragmentShader, "FRAGMENT");
		// ���Ӷ����Ƭ����ɫ����һ����ɫ�������У���������ɫ����һ�������ʱ�������ÿ����ɫ����������ӵ��¸���ɫ�������롣����������벻ƥ���ʱ�����õ�һ�����Ӵ���
		ID = glCreateProgram();
		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);
		glLinkProgram(ID);
		CheckCompileErrors(ID, "PROGRAM");
		// ɾ����ɫ������
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	};

	~Shader() {};

	void Use() {
		glUseProgram(ID);
	};

	// uniform���ߺ���
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
		//@param1 transform ���uniform ������λ��
		//@param2 1������
		//@param3 �Ƿ���Ҫת�þ���
		//@param4 GLM ��OpenGL ����������ݵķ�ʽ��һ����������GLM���ڲ�����value_ptr ����trans ����ת��
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

