#include "../../util/Id.h"
#include "../../util/dynamicarray.h"
#include <unordered_map>
#include "../renderer/shader.h"
#include "vertex.h"
#include "../HandleMap.h"
#pragma once
namespace renderer {
	using shader_id = stn::typed_id<shader>;
	struct Shaders {

		Shaders() :shader_map() {}
		void Compile(const std::string& name, const char* vert, const char* frag);
		handle::HandleMap<std::string,shader,shader_id> shader_map;
		shader_id handle_of(const std::string& Name);
		shader& shader_of(shader_id handle);
		shader& shader_of(const std::string& Name);

		vertice::vertex vertex_of();
	};
}