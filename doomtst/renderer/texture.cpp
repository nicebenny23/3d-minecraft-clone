#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"
#include "texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../util/stbiload.h"

void settextureparams(GLint texturetype) {

		glGenerateMipmap(texturetype);
		glTexParameteri(texturetype, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(texturetype, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}

texture::texture(const char* file)
{
	unsigned char* data = texdata::loadtexdata(&size.x, &size.y, file);
	glGenTextures(1,&id);

	glBindTexture(GL_TEXTURE_2D, id);
	GLint type = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, type, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
	
	settextureparams(GL_TEXTURE_2D);
}


//only works for png
texturearray::texturearray(int width, int height, array<const char*>& textures)
{
	length = textures.length;
	id = 0;
	glGenTextures(1, &id);
	apply();
	glTexImage3D(
		GL_TEXTURE_2D_ARRAY //type
		, 0                 //minmap level
		, GL_RGBA8            //color format
		, width, height, textures.length //size
		, 0,
		GL_RGBA,//color format
		GL_UNSIGNED_BYTE, nullptr);//arr
	array<unsigned char*> data = array<unsigned char*>();
	size = v2::Coord2d(width, height);
	for (int i = 0; i < textures.length; i++)
	{
	//colchannel not used but it hastto be given
		int twidth = 0;
		int theight = 0;
		unsigned char* datatoappend =texdata::loadtexdata(&twidth,& theight, textures[i]);
		
		if (v2::Coord2d(twidth, theight) != size)
		{
			Assert("texture is invalid");
		}
		//params,type,minmap level,xoffset,offset,zoffset,width,heigh,depgh,colortype,datatype,and data;
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height,1, GL_RGBA, GL_UNSIGNED_BYTE, datatoappend);
	
		stbi_image_free(datatoappend);
	}
	settextureparams(GL_TEXTURE_2D_ARRAY);
}

void texturearray::apply()
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}

texture::texture()
{
	id = -1;
}

void texture::apply()
{
	if (id==-1)
	{
		Assert("cant apply invalid texture");
	}
	glBindTexture(GL_TEXTURE_2D, id);
}
void texture::destroy()
{
	if (id == -1)
	{
		Assert("cant delete invalid texture");
	}
	glDeleteTextures(1, &id);

}
