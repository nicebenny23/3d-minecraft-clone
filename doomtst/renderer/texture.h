
#include <GLFW/glfw3.h>
#include "../util/dynamicarray.h"
#include "../util/debug.h"
#ifndef texture_h
#define texture_h
using namespace dynamicarray;
enum textype
{
	png = 1,
	jpeg =2,



};
struct texture
{
	texture(const char* file,textype png);
	texture();
	void apply();
	void destroy();
	
	GLuint id;

};
struct texturearray
{
	GLuint id;
	texturearray(int width,int height, array<const char*> textures);
	void apply();

	texturearray() = default;
};


#endif // !texture_h
