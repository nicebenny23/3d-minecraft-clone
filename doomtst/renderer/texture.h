#include "../util/vector2.h"
#include <GLFW/glfw3.h>
#include "../util/dynamicarray.h"
#include "../debugger/debug.h"
#ifndef texture_h
#define texture_h
using namespace dynamicarray;

struct texture
{
	texture(const char* file);
	texture();
	void apply();
	void destroy();
	GLuint id;
	v2::Coord2d size;
};
struct texturearray
{
	GLuint id;
	texturearray(int width,int height, array<const char*>& textures);
	void apply();
	int length;
	v2::Coord2d size;
	texturearray() = default;
};


#endif // !texture_h
