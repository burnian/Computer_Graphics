/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 02/18/2020, 17:32
*@Last Modify: 03/29/2020, 16:42
*@Desc: ͨ�ù��ߺ���
*********************************************************/
#pragma once
#include <glad/glad.h> 
#include "stb_image.h" // һ�����е�ͼƬ���ؿ�
#include <iostream>
#include <vector>
#include <string>

namespace utils {
	// stbi_set_flip_vertically_on_load(isVerFlip); ����ת
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform;
	// FileSystem::getPath("resources/textures/container.jpg").c_str();
	GLuint LoadTexture(GLchar const* path) {
		GLuint textureID;
		//@para1 �������������
		glGenTextures(1, &textureID);

		GLint width, height, nrComponents;
		GLboolean *data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data) {
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			//@para1 ����ΪGL_TEXTURE_2D��ζ�Ż������뵱ǰ�󶨵����������ͬһ��Ŀ���ϵ������κΰ󶨵�GL_TEXTURE_1D��GL_TEXTURE_3D���������ܵ�Ӱ�죩
			//@para2 �༶��Զ����ļ���0�ǻ�������
			//@para3 �������ʽ�����ǵ�ͼ��ֻ��RGBֵ���������Ҳ��������ΪRGBֵ��
			//@para4 ���յ�������
			//@para5 ���յ�����߶�
			//@para6 ����0����ʷ�������⣩
			//@para7 Դͼ�ĸ�ʽ
			//@para8 Դͼ����������
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			// Ϊ����������û��Ʒ�ʽ
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// Ϊ����������ù��˷�ʽ
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			std::cout << "Texture failed to load at path: " << path << std::endl;
		}
		stbi_image_free(data);

		return textureID;
	}

	// skybox
	GLuint LoadCubemap(const std::vector<std::string> faces) {
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		GLint width, height, nrChannels;
		for (GLuint i = 0; i < faces.size(); i++) {
			GLboolean *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else {
				std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			}
			stbi_image_free(data);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}

	// 
	GLuint TextureFromFile(const GLchar *path, const std::string &directory, bool gamma = false) {
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		GLuint textureID;
		glGenTextures(1, &textureID);

		GLint width, height, nrComponents;
		GLboolean *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data) {
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			std::cout << "Texture failed to load at path: " << path << std::endl;
		}
		stbi_image_free(data);

		return textureID;
	}

	// uniform block object to store projection
	void ConfigureUBO(const glm::mat4 &projection) {
		// uniform block object
		GLuint mat4size = sizeof(glm::mat4);
		GLuint UBOMatrices;
		glGenBuffers(1, &UBOMatrices);
		glBindBuffer(GL_UNIFORM_BUFFER, UBOMatrices);
		glBufferData(GL_UNIFORM_BUFFER, mat4size, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, mat4size, glm::value_ptr(projection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		// 0��binding pointָ��UBO��0��mat4���������
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBOMatrices, 0, mat4size);
	}

}
