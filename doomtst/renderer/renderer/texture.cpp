#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../../util/stbiload.h"

void settextureparams(GLint texturetype) {

		glGenerateMipmap(texturetype);
		glTexParameteri(texturetype, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(texturetype, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}

Texture2D::Texture2D(const char* file,const char* name) 
{
	Name= name;
	type = Tex2d;
	unsigned char* data = texdata::loadtexdata(&size.x, &size.y, file);
	glGenTextures(1,&id);

	glBindTexture(GL_TEXTURE_2D, id);
	GLint type = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, type, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
	
	settextureparams(GL_TEXTURE_2D);
}


TextureArray::TextureArray()
{
	type = TexArray;
	size = v2::Coord2(0, 0);
	length = 0;
	
}
//only works for png
TextureArray::TextureArray( array<const char*>& textures, const char* name)
{
	Name = name;
	type = TexArray;
	length = textures.length();
	id = 0;
	glGenTextures(1, &id);
	apply();
	if (textures.length()!=0)
	{
		size = texdata::GetImgSize(textures[0]);
	}


	glTexImage3D(
		GL_TEXTURE_2D_ARRAY //type
		, 0                 //minmap level
		, GL_RGBA8            //color format
		, size.x, size.y, textures.length() //size
		, 0,
		GL_RGBA,//color format
		GL_UNSIGNED_BYTE, nullptr);//arr
	array<unsigned char*> data = array<unsigned char*>();

	for (int i = 0; i < textures.length(); i++)
	{
	//colchannel not used but it hastto be given
		v2::Coord2 ImgSize;
		unsigned char* datatoappend = texdata::loadtexdata(&ImgSize.x, &ImgSize.y, textures[i]);
		if (ImgSize != size)
		{
		throw	std::logic_error("texture is invalid");
		}
		//params,type,minmap level,xoffset,offset,zoffset,width,heigh,depgh,colortype,datatype,and data;
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, size.x, size.y,1, GL_RGBA, GL_UNSIGNED_BYTE, datatoappend);
	
		stbi_image_free(datatoappend);
	}
	settextureparams(GL_TEXTURE_2D_ARRAY);
}

void TextureArray::apply() const
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}

void TextureArray::destroy()
{
	if (id == 0)
	{
		throw std::logic_error("cant delete invalid texture");
	}
	glDeleteTextures(1, &id);
	id = 0;
}

Texture2D::Texture2D() {
	
	size = v2::Coord2(0, 0);
	type = Tex2d;
}


void Texture2D::apply() const
{
	if (id==0)
	{
		throw std::logic_error("cant apply invalid texture");
	}
	glBindTexture(GL_TEXTURE_2D, id);
}
void Texture2D::destroy()
{
	if (id == 0)
	{
		throw std::logic_error("cant Delete Invalid Texture");
	}
	glDeleteTextures(1, &id);
	id = 0;
}

void ITexture::apply() const{
}

void ITexture::destroy()
{
}
