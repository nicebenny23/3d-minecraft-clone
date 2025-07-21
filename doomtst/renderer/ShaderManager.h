#include "../util/Id.h"
#include "../util/dynamicarray.h"
#include <unordered_map>
#
#include "../renderer/shader.h"
#pragma once
namespace Shaders {

	struct ShaderManager{

		ShaderManager() :string_to_id(std::unordered_map<std::string, size_t>()),shaderlist() {}
		void Compile(const std::string& name, const char* vert, const char* frag);
		std::unordered_map<std::string,size_t> string_to_id;
		stn::array<shader> shaderlist;
		Ids::Id get_handle(std::string Name);
		shader& get_shader(Ids::Id handle);
		
	};
}