#include "vertexobject.h"	
#include "Vao.h"
#pragma once
namespace renderer {
	struct Mesh {
		Mesh() = delete;
		bool filled() const {
			return (length != 0);
		}
		size_t length;
		renderer::Vao vao;
		renderer::Ebo ebo;
		renderer::Vbo vbo;
		Mesh(size_t len, Vao vao_for, Ebo ebo_for, Vbo vbo_for)
			: length(len), vao(vao_for), ebo(ebo_for), vbo(vbo_for) {
		}

	};
}


