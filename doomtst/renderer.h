#ifndef Renderer_HPP
#define Renderer_HPP
#include "dynamicarray.h"
#include <glad/glad.h>
#include "shader.h"
#include <glm/mat4x4.hpp>
#include "vector3.h"
#include <glm/gtc/matrix_transform.hpp>
#include "texture.h"
#include "vertexobject.h"
using namespace vobj;
#define texturesize 6
enum shadertype {
	normal=0

};
namespace renderer {
	extern vao VAO;
	extern vbuf VBO, ibo;
	void renderquadlist(dynamicarray::array<float>& pointlist, dynamicarray::array<unsigned int>& indicelist);
	void load();
	void setviewmatrix(glm::mat4 viewmat);
	extern glm::mat4 proj,view;
	extern int currshader;
	extern dynamicarray::array<shader> shaderlist;


}
#endif

