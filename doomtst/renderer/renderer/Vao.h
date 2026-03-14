#pragma once
#include <glad/glad.h>
namespace renderer {


	struct Vao {

		GLuint id;
		Vao(GLuint vao_id) :id(vao_id) {

		}
		
	};
}

