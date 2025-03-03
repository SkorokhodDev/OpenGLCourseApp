#pragma once

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Texture.h"

class Model
{
public:
	Model();
	
	void LoadModel(const std::string FileName);
	void RenderModel();
	void ClearModel();

	~Model();

private:

	void LoadNode(const aiNode* Node, const aiScene *Scene);
	void LoadMesh(const aiMesh* Mesh, const aiScene* Scene);
	void LoadMaterials(const aiScene* Scene);

	std::vector<Mesh*> MeshList;
	std::vector<Texture*> TextureList;
	std::vector<unsigned int> MeshToTexture;
};

