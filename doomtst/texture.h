#include <glad/glad.h>
#include <GLFW/glfw3.h>
#ifndef texture_h
#define texture_h
enum textype
{
	png = 1,
	jpeg =2,



};
struct texture
{
	texture(const char* file,textype png);
	texture();


	
	GLuint id;

};



#endif // !texture_h
