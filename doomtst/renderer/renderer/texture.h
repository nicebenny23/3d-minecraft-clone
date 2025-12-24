#include "../../math/vector2.h"
#include <GLFW/glfw3.h>
#include "../../util/dynamicarray.h"
#include <glad/glad.h>
#pragma once 
using namespace stn;
enum TextureType {
	TexArray = GL_TEXTURE_2D_ARRAY,
	Tex2d = GL_TEXTURE_2D,
	Tex3d = 2,
	TextureNull = 3,
}; namespace TextureManager {
	class TextureManager; // Forward declaration
}

struct ITexture
{
	GLuint id;
	
	TextureType type;
	ITexture() {
		type = TextureNull;
		id = 0;
		Name="";
	};
	std::string Name;

	virtual void apply() const;
	virtual void destroy();
	friend class TextureManager::TextureManager;
};
struct Texture2D:ITexture
{	Texture2D();
	v2::Coord2 size;

	void apply() const override;
	void destroy() override;
	Texture2D(const char* file, const char* name);
	
};
struct TextureArray:ITexture
{
	TextureArray();
	int length;
	v2::Coord2 size;

	void apply() const override;
	void destroy() override;
	TextureArray(array<const char*>& textures, const char* name);
	
};


 // !texture_h
