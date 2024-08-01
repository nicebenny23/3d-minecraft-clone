#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"
#include "texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


void settextureparams() {
	//  glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY, 10);


	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);




}
texture::texture(const char* file, textype imgtype)
{
	int width;
	int height;

	int colchannel=0;

	unsigned char* data = stbi_load(file, &width, &height, &colchannel, STBI_rgb_alpha);

	for (int i = 0; i < 10; i++)
	{
		if (data==nullptr)
		{
			data = stbi_load(file, &width, &height, &colchannel, STBI_rgb_alpha);
		}
		else
		{
			break;
		}
	}
	if (data==nullptr)
	{
		
			std::cerr << "Failed to load texture: " << stbi_failure_reason() << std::endl;
		
		Assert("texture doesent exist");

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, type, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	settextureparams();
}

texture::texture()
{
	id = -1;
}

void texture::apply()
{


	glBindTexture(GL_TEXTURE_2D, id);
}
void texture::destroy()
{
	glDeleteTextures(1,&id);

}
//only works for png
texturearray::texturearray(int width, int height, array<const char*> textures)
{
	array<unsigned char*> data = array<unsigned char*>();

	for (int i = 0;i < textures.length;i++) {
		//colchannel not used but it hastto be given
		int colchannel = 0;
		unsigned char* datatoappend = stbi_load(textures[i], &width, &height, &colchannel, STBI_rgb_alpha);
		if (datatoappend != nullptr)
		{
			data.append(datatoappend);
		}
		else
		{
			Assert("texture doesent exist");

		}

	}
	id = 0;
	int minmaplevel = 2;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);

//glTexStorage3D(GL_TEXTURE_2D_ARRAY, minmaplevel, GL_RGBA8, width, height, 1);
	
	glTexImage3D(
		GL_TEXTURE_2D_ARRAY //type
		, 0                 //minmap level
		, GL_RGBA8            //color format
		, width, height,data.length //size
		,0,
		GL_RGBA,//color format
		GL_UNSIGNED_BYTE, nullptr);//array of unsigned bytes,null because empty
	//glTexImage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGB, width/2, height/2, data.length, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	for (int i = 0; i < textures.length; i++)
	{//params,type,minmap level,xoffset,offset,zoffset,width,heigh,depgh,colortype,datatype,and data;
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height,1, GL_RGBA, GL_UNSIGNED_BYTE, data[i]);
	

	}
	
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	//glUniform1i(id, 0);
	settextureparams();
}

void texturearray::apply()
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}
