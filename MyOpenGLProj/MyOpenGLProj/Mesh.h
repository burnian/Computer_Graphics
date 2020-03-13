/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 02/17/2020, 14:36
*@Last Modify: 03/13/2020, 13:34
*@Desc: Mesh
*********************************************************/
#pragma once
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct Texture {
	GLuint id;
	std::string type;
	std::string path;  // we store the path of the texture to compare with other textures
};

class Mesh {
public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
		: vertices(vertices), indices(indices), textures(textures)
	{
		SetupMesh();
	};

	// 一个mesh最多可以有32张纹理，每种纹理默认从1开始命名，如：texture_diffuse1
	//@param texUnitOffset 可使用的纹理单元起始编号
	void Draw(const Shader &shader, GLuint texUnitOffset) {
		GLuint diffuseNr = 1;
		GLuint specularNr = 1;
		GLuint normalNr = 1;
		GLuint heightNr = 1;
		for (GLuint i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + texUnitOffset + i); // activate proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
			std::string number;
			std::string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);
			else if (name == "texture_normal")
				number = std::to_string(normalNr++);
			else if (name == "texture_height")
				number = std::to_string(heightNr++);
			
			shader.SetInt("material." + name + number, texUnitOffset + i); // 想通过glUniform1f接口把一个float值赋给sampler2D无法成功，sampler2D貌似只接受int
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);

		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	};

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
private:
	// 步骤错了，渲染就会出问题
	void SetupMesh() {
		// step 1
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		// step 2
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		// step 3
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	};

	/*  Render data  */
	GLuint VAO, VBO, EBO;
};