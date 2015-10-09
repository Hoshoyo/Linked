#include "Texture.h"
#include "stb_image.h"

Texture::Texture(std::string fileName) : Texture(fileName, 0){}

Texture::Texture(std::string fileName, float bias) : ImageLoader(fileName, 4)
{
	this->bias = bias;
	this->fileName = fileName;
	textureID = genGLTexture();
}

Texture::Texture(){}
Texture::~Texture(){}

std::string Texture::getFilename()
{
	return this->fileName;
}

GLuint Texture::genGLTexture()
{
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, loadedImage);

	stbi_image_free(loadedImage);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	if (bias < 0)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, (GLfloat)bias);
	}
	glGenerateMipmap(GL_TEXTURE_2D);

	return textureID;
}

GLuint Texture::genGLNullTexture(int width, int height)
{
	// Generate texture
	GLuint texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texColorBuffer;
}

DynamicTexture::DynamicTexture(int width, int height, bool mipmap, float bias)
{
	this->fileName = "";
	this->width = width;
	this->height = height;
	this->channels = 4;
	this->bias = bias;
	//textureID = genDynamicGLTexture(mipmap);
}

DynamicTexture::~DynamicTexture()
{
	//stbi_image_free(loadedImage);
}

GLuint DynamicTexture::genDynamicGLTexture()
{
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, loadedImage);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, (GLfloat)this->bias);
	glGenerateMipmap(GL_TEXTURE_2D);

	return textureID;
}