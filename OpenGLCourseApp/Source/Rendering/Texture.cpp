#define STB_IMAGE_IMPLEMENTATION

#include "Texture.h"

#include "../../stb_image.h"

Texture::Texture() : TextureID(0), width(0), height(0), bitDepth(0), FileLocation(0)
{
}

Texture::Texture(const char* FilePath, bool AutoLoad, bool AlphaChannel) : Texture()
{
	FileLocation = FilePath;
	if(AutoLoad)
	{
		LoadTexture(AlphaChannel);
	}
}

bool Texture::LoadTexture(bool AlphaChannel)
{
	// Array of bytes for representing texture data
	unsigned char* TextData = stbi_load(FileLocation, &width, &height, &bitDepth, 0);
	
	if (!TextData)
	{
		printf("Failed to find: %s\n", FileLocation);
		return false;
	}

	// Generating our texture
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	
	/** Determine how the image wraping in S axis(S = x axis, T = y axis).
	For exempale when we out of texture border ( 1.2 coord) */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// When we zooming on the image, pixels are blanding. GL_NEAREST = PixelArt.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(AlphaChannel)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextData);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, TextData);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(TextData);

	return true;
}

void Texture::UseTexture()
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureID);
}

void Texture::ClearTexture()
{
	glDeleteTextures(1, &TextureID);
	TextureID = width = height = bitDepth = 0;
	FileLocation = "";
}

Texture::~Texture()
{
	ClearTexture();
}
