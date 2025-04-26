#include "../util/vector2.h"
#include <GLFW/glfw3.h>
#include "../util/dynamicarray.h"
#include "../debugger/debug.h"
#ifndef texture_h
#define texture_h
using namespace dynamicarray;
enum TextureType {
	TexArray = 0,
	Tex2d = 1,
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
		id = -1;
		Name="";
	};
	std::string Name;
private:
	virtual void apply();
	virtual void destroy();
	friend class TextureManager::TextureManager;
};
struct Texture2D:ITexture
{	Texture2D();
	v2::Coord2d size;
private:
	void apply() override;
	void destroy();
	Texture2D(const char* file, const char* name);
	friend class TextureManager::TextureManager;

};
struct TextureArray:ITexture
{
	TextureArray();
	int length;
	v2::Coord2d size;
private:
	void apply() override;
	void destroy() override;
	TextureArray(array<const char*>& textures, const char* name);
	friend class TextureManager::TextureManager;

};


#endif // !texture_h
