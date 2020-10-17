/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 08/30/2019, 13:36
*@Last Modify: 03/29/2020, 16:41
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
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = nullptr) {
		// ���ļ�·���л�ȡ����/Ƭ����ɫ��
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
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
		// ������ɫ��
		const GLchar* vShaderCode = vertexCode.c_str();
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);// @para2 ���ݵ�Դ���ַ�������
		glCompileShader(vertexShader);
		CheckCompileErrors(vertexShader, "VERTEX");
		glAttachShader(ID, vertexShader);
		glDeleteShader(vertexShader);
		// ƬԪ��ɫ��
		const GLchar* fShaderCode = fragmentCode.c_str();
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);
		CheckCompileErrors(fragmentShader, "FRAGMENT");
		glAttachShader(ID, fragmentShader);
		glDeleteShader(fragmentShader);
		// ������ɫ��
		if (geometryPath) {
			const GLchar* gShaderCode = geometryCode.c_str();
			GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometryShader, 1, &gShaderCode, NULL);
			glCompileShader(geometryShader);
			CheckCompileErrors(geometryShader, "GEOMETRY");
			glAttachShader(ID, geometryShader);
			glDeleteShader(geometryShader);
		}
		// ���Ӷ����Ƭ����ɫ����һ����ɫ�������У���������ɫ����һ�������ʱ�������ÿ����ɫ����������ӵ��¸���ɫ�������롣����������벻ƥ���ʱ�����õ�һ�����Ӵ���
		glLinkProgram(ID);
		CheckCompileErrors(ID, "PROGRAM");
	};

	~Shader() {};

	// �����shader ����
	void Use() {
		glUseProgram(ID);
	};

	// ��shader�����е�uniform block��binding point����������UB = uniform block
	//@param name shader�е�uniform block��
	void BindUniformBlock(const std::string &UBName, GLuint UBbindingPoint) {
		GLuint UBIndex = glGetUniformBlockIndex(ID, UBName.c_str());
		glUniformBlockBinding(ID, UBIndex, UBbindingPoint);
	};

	// ��ƽ�й�
	void SetupDirLight(glm::vec3 dir = glm::vec3(1.0f, 1.0f, 0.0f), GLfloat luminance = 1.0f) {
		Use();
		SetVec3("dirLight.direction", dir);
		SetVec3("dirLight.ambient", glm::vec3(0.5f) * glm::vec3(luminance));
		SetVec3("dirLight.diffuse", glm::vec3(1.0f) * glm::vec3(luminance));
		SetVec3("dirLight.specular", glm::vec3(1.0f) * glm::vec3(luminance));
	}

	// �򿪾۹��
	void TurnOnSpotLight(GLfloat luminance = 1.0f) {
		Use();
		SetFloat("spotLight.innerCos", glm::cos(glm::radians(12.5f)));
		SetFloat("spotLight.outerCos", glm::cos(glm::radians(15.0f)));
		SetVec3("spotLight.ambient", glm::vec3(0.0f) * glm::vec3(luminance));
		SetVec3("spotLight.diffuse", glm::vec3(1.0f) * glm::vec3(luminance));
		SetVec3("spotLight.specular", glm::vec3(1.0f) * glm::vec3(luminance));
		SetFloat("spotLight.constant", 1.0f); // ��Ч����˥����Χ50
		SetFloat("spotLight.linear", 0.09f);
		SetFloat("spotLight.quadratic", 0.032f);
	}

	void MoveSpotLight(glm::vec3 position, glm::vec3 front) {
		SetVec3("spotLight.position", position);
		SetVec3("spotLight.direction", front);
	}

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
		//@param1 transform ���uniform ������λ��
		//@param2 1������
		//@param3 �Ƿ���Ҫת�þ���
		//@param4 GLM ��OpenGL ����������ݵķ�ʽ��һ����������GLM���ڲ�����value_ptr ����trans ����ת��
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
