/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 10/29/2020, 16:21
*@Last Modify: 11/04/2020, 23:44
*@Desc: generating shadow depth map
*********************************************************/
#pragma once
#include <glad/glad.h>
#include <iostream>

//////////////////////////////
class ShadowMap {
public:
	ShadowMap() {
		glGenFramebuffers(1, &fbo);
	};

	~ShadowMap() {};

	bool InitCubeMap(const GLuint& shadowWidth, const GLuint& shadowHeight, const GLenum& texUnit) {
		glGenTextures(1, &_cubeMap);
		glActiveTexture(texUnit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _cubeMap);
		for (GLuint i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// bind 6 GL_TEXTURE_2D textures of the shadow depth cube map to the FBO one by one.
		//for (GLuint i = 0; i < 6; i++) {
		//	GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
		//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, face, depthCubemap, 0);
		//	BindViewMatrix(lightViewMatrices[i]);
		//	RenderSceneShadow();
		//}

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _cubeMap, 0);
		// Disable writes to the color buffer
		glDrawBuffer(GL_NONE);
		// Disable reads from the color buffer
		glReadBuffer(GL_NONE);
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "FRAMEBUFFER error, status: 0x" << status << "\n";
			return false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	};

	bool Init2DMap(const GLuint& shadowWidth, const GLuint& shadowHeight, const GLenum& texUnit) {
		glGenTextures(1, &_2DMap);
		glActiveTexture(texUnit);
		glBindTexture(GL_TEXTURE_2D, _2DMap);
		// we only care about depth, so the texture storing type sets to GL_DEPTH_COMPONENT
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// Solving the problem that an object is shadowed with nothing occluding it by setting the marginal value of the shadow depth texture to 1.
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _2DMap, 0);
		// Disable writes to the color buffer
		glDrawBuffer(GL_NONE);
		// Disable reads from the color buffer
		glReadBuffer(GL_NONE);
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "FRAMEBUFFER error, status: 0x" << status << "\n";
			return false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	};

	GLuint fbo;

private:
	GLuint _cubeMap;
	GLuint _2DMap;
};
