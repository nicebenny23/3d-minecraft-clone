#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>



texture::texture(const char* file, textype imgtype)
{
	int width;
	int height;

	int colchannel;
	
	unsigned char* data = stbi_load(file, &width,& height, &colchannel, 0);

	if (data==nullptr)
	{
		std::cout << "texture doesent exist";
		std::abort();
	}
	glGenTextures(1,&id);

	glBindTexture(GL_TEXTURE_2D, id);
	int type = 0;
	if (imgtype ==png)
	{
		type = GL_RGBA;
	}
	if (imgtype == jpeg)
	{
		type = GL_RGB;
	
	}
	glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}

texture::texture()
{
}
