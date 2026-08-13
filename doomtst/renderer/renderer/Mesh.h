#include "vertexobject.h"	
#pragma once
namespace renderer {
	struct GpuMesh{
		GpuMesh() = delete;
		bool filled() const {
			return (length != 0);
		}
		size_t length;
		renderer::Vao vao;
		renderer::Ebo ebo;
		bool operator==(const GpuMesh& other) const= default;
		bool operator!=(const GpuMesh& other) const = default;
		renderer::Vbo vbo;
		GpuMesh(size_t len, Vao vao_for, Ebo ebo_for, Vbo vbo_for)
			: length(len), vao(vao_for), ebo(ebo_for), vbo(vbo_for) {
		}

	};
}


