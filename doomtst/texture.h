#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "dynamicarray.h"
#include "debug.h"
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

	
	GLuint id;

};
struct texturearray
{
	GLuint id;
	texturearray(int width,int height, array<const char*> textures, textype imgtype);
	void apply();
};


#endif // !texture_h
