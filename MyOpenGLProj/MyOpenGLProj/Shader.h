#pragma once
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
public:
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	~Shader();
	void Use();
	// uniform¹¤¾ßº¯Êý
	void SetBool(const std::string &name, bool value) const;
	void SetInt(const std::string &name, GLint value) const;
	void SetFloat(const std::string &name, GLfloat value) const;
private:
	void CheckCompileErrors(GLuint shader, std::string type);
public:
	GLuint ID;
};

