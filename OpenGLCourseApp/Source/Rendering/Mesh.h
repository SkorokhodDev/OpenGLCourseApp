#pragma once

#include "GL/glew.h"

class Mesh
{
public:
	Mesh();
	~Mesh();

	void CreateMesh(GLfloat* vertices, unsigned int* indeces, unsigned int NumOfVertices, unsigned int NumOfIndices);
	void RenderMesh();
	void ClearMesh();

private:
	GLuint VAO, VBO, IBO;
	GLsizei IndexCount;
};

