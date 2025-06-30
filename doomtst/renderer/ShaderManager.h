
#include <unordered_map>
#include "../renderer/shader.h"
#pragma once
namespace Shaders {
	struct ShaderManager{

		ShaderManager() {
			shaderlist = std::unordered_map<std::string, shader*>();
		};
		void Compile(const std::string& name, const char* vert, const char* frag);
		void Bind(const std::string Name);
		std::unordered_map<std::string, shader*> shaderlist;
		shader* BoundShader;
		
	};
}