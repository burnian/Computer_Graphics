/*********************************************************
*@Author: Burnian Zhou
*@Create Time: 02/17/2020, 20:38
*@Last Modify: 03/26/2020, 01:29
*@Desc: model
*********************************************************/
#pragma once
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>

#include "Shader.h"
#include "Mesh.h"
#include "utils.h"

class Model {
public:
	Model(std::string const &path, bool gamma = false)
		: gammaCorrection(gamma)
	{
		LoadModel(path);
	}

	// һ��ģ��ͨ���ж��mesh
	//@param1 ���λ���ʹ�õ���ɫ��
	//@param2 һ��draw call ���ã�����model ����
	//@param3 ��ʹ�õ�����Ԫ��ʼ���
	void Draw(const Shader &shader, GLuint amount = 1, GLuint texUnitOffset = 0) {
		for (Mesh mesh : meshes)
			mesh.Draw(shader, amount, texUnitOffset);
	};

	std::vector<Texture> texturesLoaded;
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;
private:
	void LoadModel(std::string const &path) {
		Assimp::Importer importer;
		// aiProcess_Triangulate����ģ���д��ڷ���������Assimp ����������������ȫ��ת��Ϊ�����档
		// aiProcess_FlipUVs��������������y �ᷭת��
		// aiProcess_GenNormals�����ģ�͵Ķ�������û�з��ߣ�����֮��
		// aiProcess_SplitLargeMeshes�������Ⱦ�����Ķ������������ֵ������Զ���mesh �ָ�ɽ�С��mesh��
		// aiProcess_OptimizeMeshes������С��mesh �ϳ�һ���ϴ��mesh���Լ���draw call��
		const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		// ���complete����mFlags ��INCOMPLETE ����һλΪ0������scene->mFlags Ϊ0������Ϊ0
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));

		ProcessNode(scene->mRootNode, scene);
	};

	void ProcessNode(aiNode *node, const aiScene *scene) {
		// process all the node's meshes (if any)
		for (GLuint i = 0; i < node->mNumMeshes; i++) {
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(ProcessMesh(mesh, scene));
		}
		// then do the same for each of its children
		for (GLuint i = 0; i < node->mNumChildren; i++) {
			ProcessNode(node->mChildren[i], scene);
		}
	};

	Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene) {
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;

		// process vertex positions, normals and texture coordinates
		for (GLuint i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			// a model can have up to 8 different texture coordinates per vertex, we only care about the first set of texture coordinates.
			if (mesh->mTextureCoords[0])
				vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
			vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

			vertices.push_back(vertex);
		}
		// process indices
		for (GLuint i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (GLuint j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// process materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		// 1. diffuse maps
		std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		return Mesh(vertices, indices, textures);
	};

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
		std::vector<Texture> textures;
		for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);
			bool isSkip = false;
			for (GLuint j = 0; j < texturesLoaded.size(); j++) {
				if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0) {
					textures.push_back(texturesLoaded[j]);
					isSkip = true;
					break;
				}
			}
			if (!isSkip) {   // if texture hasn't been loaded already, load it
				Texture texture;
				texture.id = utils::TextureFromFile(str.C_Str(), directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				texturesLoaded.push_back(texture);
			}
		}
		return textures;
	}
};
