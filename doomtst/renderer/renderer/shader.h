
#pragma once
#include <string>
#include <stdexcept>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include "../../math/vector2.h"
#include "../../math/vector3.h"
#include "../../assets/Assets.h"
#include "GlUtil.h"
namespace renderer {
	struct Shader :assets::Asset {

		GLint id;
		Shader() {
			id = 0;
		}
		GLint uniformlocation(const char* name) const {
			if (id == 0) {
				throw std::logic_error("invalid block_id");
			}
			
			GLint location = glGetUniformLocation(id, name);
			if (location == -1) {
				std::string msg = "Uniform '" + std::string(name) + "' not found or not active in shader";
				throw std::logic_error(msg);
			}
			return location;

		}
		Shader(GLint shader_id) :id(shader_id) {
		}
	};

	struct shader_descriptor {
		using asset_type = Shader;
		shader_descriptor(const std::string& name, const std::string& vertex, const std::string& fragment)
			: shader_name(name), vertex_path(vertex), fragment_path(fragment) {
		}
		std::string_view name() const {
			return shader_name;
		}
		std::string shader_name;
		std::string vertex_path;
		std::string fragment_path;

		bool operator==(const shader_descriptor& other) const noexcept {
			return shader_name == other.shader_name
				&& vertex_path == other.vertex_path
				&& fragment_path == other.fragment_path;
		}
		shader_descriptor() = default;
	};
}
namespace std {
	template<>
	struct hash<renderer::shader_descriptor> {
		std::size_t operator()(const renderer::shader_descriptor& s) const noexcept {
			std::hash<std::string> hasher;
			return hasher(s.shader_name) + hasher(s.vertex_path) + hasher(s.fragment_path);
		}
	};
}

#include <fstream>
#include <iostream>
namespace renderer {
	
	inline int compile_shader(const char* name, GLint shadertype) {
		std::ifstream shaderstream(name);
		if (shaderstream.fail()) {
			std::cout << "shader filestream error" << "\n" << "could not find shader named " << name;
		}
		std::string shadercode = std::string(std::istreambuf_iterator<char>(shaderstream), std::istreambuf_iterator<char>());
		shaderstream.close();
		const char* c_strcode = shadercode.c_str();
		unsigned int shaderid = glad_glCreateShader(shadertype);
		glShaderSource(shaderid, 1, &c_strcode, NULL);
		glCompileShader(shaderid);
		gl_util::shaderstatuscheck(shaderid, GL_COMPILE_STATUS, name);
		return shaderid;
	}

	using shader_id = assets::AssetHandle<Shader>;

}