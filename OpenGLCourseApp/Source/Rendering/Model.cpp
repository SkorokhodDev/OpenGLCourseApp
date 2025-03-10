#include "Model.h"

#include <GL/glew.h>

Model::Model()
{
}

void Model::LoadModel(const std::string FileName)
{
	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(FileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

	if (!Scene) {
		printf("Model failed to load: %s, %s", Importer.GetErrorString(), FileName.c_str());
		return;
	}

	LoadNode(Scene->mRootNode, Scene);
	LoadMaterials(Scene);
}



Model::~Model()
{
	ClearModel();
}


void Model::ClearModel()
{
	for (size_t i = 0; i < MeshList.size(); i++)
	{
		if (MeshList[i])
		{
			delete MeshList[i];
			MeshList[i] = nullptr;
		}
	}
	for (size_t i = 0; i < TextureList.size(); i++)
	{
		if (TextureList[i])
		{
			delete TextureList[i];
			TextureList[i] = nullptr;
		}
	}
}

void Model::RenderModel()
{
	for (size_t i = 0; i < MeshList.size(); i++)
	{
		unsigned int MaterialIndex = MeshToTexture[i];

		if (MaterialIndex < TextureList.size() && TextureList[MaterialIndex])
		{
			TextureList[MaterialIndex]->UseTexture();
		}
		MeshList[i]->RenderMesh();
	}
}

void Model::LoadNode(const aiNode* Node, const aiScene* Scene)
{
	for (size_t i = 0; i < Node->mNumMeshes; i++)
	{
		LoadMesh(Scene->mMeshes[Node->mMeshes[i]], Scene);
	}

	for (size_t i = 0; i < Node->mNumChildren; i++)
	{
		LoadNode(Node->mChildren[i], Scene);
	}

}


void Model::LoadMesh(const aiMesh* Mesh, const aiScene* Scene)
{
	std::vector<GLfloat> Vertices;
	std::vector<unsigned int> Indices;

	for (size_t i = 0; i < Mesh->mNumVertices; i++)
	{
		Vertices.insert(Vertices.end(), { Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z });

		if (Mesh->mTextureCoords[0]) 
		{
			Vertices.insert(Vertices.end(), 
				{ Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y});
		}
		else 
		{
			Vertices.insert(Vertices.end(), { 0.0f, 0.0f });
		}
		Vertices.insert(Vertices.end(), { -Mesh->mNormals[i].x, -Mesh->mNormals[i].y, -Mesh->mNormals[i].z });
	}
	for (size_t i = 0; i < Mesh->mNumFaces; i++) // Index bufer
	{
		aiFace Face = Mesh->mFaces[i];
		for (size_t j = 0; j < Face.mNumIndices; j++)
		{
			Indices.push_back(Face.mIndices[j]);
		}
	}

	class Mesh* NewMesh = new class Mesh();
	NewMesh->CreateMesh(&Vertices[0], &Indices[0], Vertices.size(), Indices.size());
	MeshList.push_back(NewMesh);
	MeshToTexture.push_back(Mesh->mMaterialIndex);
}

// Run through all materials in there and place them in appropriate location in our texture
void Model::LoadMaterials(const aiScene* Scene)
{
	TextureList.resize(Scene->mNumMaterials);

	for (size_t i = 0; i < Scene->mNumMaterials; i++)
	{
		aiMaterial* Material = Scene->mMaterials[i];
		TextureList[i] = nullptr;

		if (Material->GetTextureCount(aiTextureType_DIFFUSE))
		{
			aiString Path;
			if (Material->GetTexture(aiTextureType_DIFFUSE, 0, &Path) == AI_SUCCESS)
			{
				int index = std::string(Path.data).rfind("\\");
				std::string FileName = std::string(Path.data).substr(index + 1);

				std::string TexturePath = std::string("Resources/Textures/") + FileName;

				TextureList[i] = new Texture(TexturePath.c_str());

				if (!TextureList[i]->LoadTexture(false))
				{
					printf("Failed to load texutre at: %s", TexturePath.c_str());
					delete TextureList[i];
					TextureList[i] = nullptr;
					return;
				}
			}
		}
		if (!TextureList[i])
		{
			TextureList[i] = new Texture("Resources/Textures/plain.png");
			TextureList[i]->LoadTexture(false);
		}

	}
}
