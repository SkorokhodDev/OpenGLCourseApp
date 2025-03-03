#include "Mesh.h"

Mesh::Mesh() : VAO(0), VBO(0), IBO(0), IndexCount(0)
{
}

Mesh::~Mesh()
{
	ClearMesh();
}

void Mesh::CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int NumOfVertices, unsigned int NumOfIndices)
{
	IndexCount = NumOfIndices;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	//sizeof(indices) return value of pointr, bec we passing pointer to first element of massive
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0])*IndexCount, indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * NumOfVertices, vertices, GL_STATIC_DRAW);
	
	// Handle coords
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (sizeof(vertices[0]) * 8), (void*)0);
	glEnableVertexAttribArray(0);
	// Handle textures
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (sizeof(vertices[0]) * 8), (void*)(sizeof(vertices[0]) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (sizeof(vertices[0]) * 8), (void*)(sizeof(vertices[0]) * 5));
	glEnableVertexAttribArray(2);

	//Unbind our buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Mesh::RenderMesh()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glDrawElements(GL_TRIANGLES, IndexCount, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::ClearMesh()
{
	if (IBO != 0)
	{
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}

	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	IndexCount = 0;
}
