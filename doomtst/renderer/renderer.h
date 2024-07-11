#ifndef Renderer_HPP
#define Renderer_HPP
#include "..//renderer/renderer.h"

#include "vertexobject.h"
#include "shader.h"
#include <glm/mat4x4.hpp>
#include "../util/vector3.h"
#include <glm/gtc/matrix_transform.hpp>
#include "texture.h"
#

using namespace vobj;
#define texturesize 6

namespace renderer {
	enum shadertype {
		normal = 0,
		uishader = 1
	};
	void renderquadlist(vao VAO, vbuf ibo, vbuf VBO, dynamicarray::array<float>& pointlist, dynamicarray::array<unsigned int>& indicelist);
	void load();
	void setviewmatrix(glm::mat4 viewmat);
	extern glm::mat4 proj,view;
	extern int currshader;
	extern dynamicarray::array<shader> shaderlist;
	extern texturearray texarray;
	enum rendertype
	{
		solid = 0,
		transparent=1,
		ui=2,
	}; void changerendertype(rendertype rentype);
}
#endif

