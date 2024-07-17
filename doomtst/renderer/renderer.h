#ifndef Renderer_HPP
#define Renderer_HPP
#include "..//renderer/renderer.h"

#include "vertexobject.h"
#include "shader.h"
#include <glm/mat4x4.hpp>
#include "../util/vector3.h"
#include <glm/gtc/matrix_transform.hpp>
#include "texture.h"


using namespace vobj;
#define texturesize 6

namespace renderer {
	enum shadertype {
		normalshader = 0,
		uishader = 1,
		modelshader = 2,
		particleshader = 3,

	};
	void renderquadlist(vao VAO, vbuf ibo, vbuf VBO, dynamicarray::array<float>& pointlist, dynamicarray::array<unsigned int>& indicelist);
	void render2dquadlist(vao VAO, vbuf ibo, vbuf VBO, array<float>& pointlist, array<unsigned int>& indicelist);
	void load();
	void setviewmatrix(glm::mat4 viewmat);
	void clear();
	extern glm::mat4 proj,view;
	extern int currshader;
	extern float fov;
	extern dynamicarray::array<shader> shaderlist;
	void setrenderingmatrixes();
	
	extern texturearray texarray;
	enum rendertype
	{
		rendersolid = 0,
		rendertransparent=1,
		renderui=2,
		rendermodel= 3,
		renderparticle=4,
	}; void changerendertype(rendertype rentype);

}
#endif

