/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 02/18/2020, 17:32
*@Last Modify: 03/29/2020, 16:42
*@Desc: 通用工具函數
*********************************************************/
#pragma once
#include <glad/glad.h> 
#include "stb_image.h" // 一个流行的图片加载库
#include <iostream>
#include <vector>
#include <string>

namespace utils {
	// stbi_set_flip_vertically_on_load(isVerFlip); 纹理翻转
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform;
	// FileSystem::getPath("resources/textures/container.jpg").c_str();
	GLuint LoadTexture(GLchar const* path) {
		GLuint textureID;
		//@para1 生成纹理的数量
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
			//@para1 设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理（任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）
			//@para2 多级渐远纹理的级别，0是基本级别
			//@para3 纹理储存格式（我们的图像只有RGB值，因此我们也把纹理储存为RGB值）
			//@para4 最终的纹理宽度
			//@para5 最终的纹理高度
			//@para6 总是0（历史遗留问题）
			//@para7 源图的格式
			//@para8 源图的数据类型
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			// 为纹理对象设置环绕方式
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// 为纹理对象设置过滤方式
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
		// 0号binding point指向UBO的0到mat4的这块区域
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBOMatrices, 0, mat4size);
	}

}
