#pragma once

#include <GL/glew.h>

class Texture
{
public:
	Texture();
	Texture(const char* FilePath, bool AutoLoad = false, bool AlphaChannel = false);
	
	bool LoadTexture(bool AlphaChannel = false);
	void UseTexture();
	void ClearTexture();

	~Texture();

private:
	GLuint TextureID;
	int width, height, bitDepth;

	const char* FileLocation = nullptr;
};

