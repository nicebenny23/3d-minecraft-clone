
#include <unordered_map>
#include "../renderer/shader.h"
#pragma once
namespace Shaders {
	struct ShaderManager{

		ShaderManager() {
			shaderlist = std::unordered_map<std::string, shader*>();
		};
		void Compile(const std::string& name, const char* vert, const char* frag);
		
		std::unordered_map<std::string, shader*> shaderlist;
	
		shader& operator[](std::string Name) {
			return *shaderlist[Name];
		}
	};
}