/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 02/18/2020, 17:32
*@Last Modify: 02/18/2020, 17:34
*@Desc: ͨ�ù��ߺ���
*********************************************************/
#pragma once
#include <glad/glad.h> 
#include "stb_image.h" // һ�����е�ͼƬ���ؿ�
#include <iostream>

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

}
