
#pragma once
#include <string>
#include <stdexcept>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include "../../math/vector2.h"
#include "../../math/vector3.h"
#include "../../assets/Assets.h"
#include "GlUtil.h"
struct shader :assets::Asset {

	GLint id;
	shader() {
		id = 0;
	}
	GLint uniformlocation(const char* name) const {
		if (id == 0) {
			throw std::logic_error("invalid id");
		}
		GLint location = glGetUniformLocation(id, name);
		if (location == -1) {
			std::string msg = "Uniform '" + std::string(name) + "' not found or not active in shader '" + name + "'";
			throw std::logic_error(msg);
		}
		return location;

	}
	shader(GLint id) :id(id) {
	}
};



struct shader_load {
	using asset_type=shader;
	shader_load(const std::string& name, const std::string& vertex, const std::string& fragment)
		: shader_name(name), vertex_path(vertex), fragment_path(fragment) {
	}
	std::string_view name() const {
		return shader_name;
	}
	std::string shader_name;
	std::string vertex_path;
	std::string fragment_path;
	
	bool operator==(const shader_load& other) const noexcept {
		return shader_name == other.shader_name
			&& vertex_path == other.vertex_path
			&& fragment_path == other.fragment_path;
	}
	shader_load() = default;
};

namespace std {
	template<>
	struct hash<shader_load> {
		std::size_t operator()(const shader_load& s) const noexcept {
			std::hash<std::string> hasher;
			return hasher(s.shader_name) + hasher(s.vertex_path) + hasher(s.fragment_path);
		}
	};
}

#include <fstream>
#include <iostream>

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
	GlUtil::shaderstatuscheck(shaderid, GL_COMPILE_STATUS, name);
	return shaderid;
}

struct shader_loader{
	using load_descriptor = shader_load;
	shader_loader() {

	}
	void unload(stn::box<shader> shader) {
	
	}
	static constexpr bool immortal = true;
	stn::box<shader> load(const shader_load& load_info) {	
		int VertexShader = compile_shader(load_info.vertex_path.c_str(), GL_VERTEX_SHADER);
		int FragmentShader = compile_shader(load_info.fragment_path.c_str(), GL_FRAGMENT_SHADER);
		GLint id = glCreateProgram();
		glAttachShader(id, VertexShader);
		glAttachShader(id, FragmentShader);
		glLinkProgram(id);
		GlUtil::shaderstatuscheck(id, GL_LINK_STATUS, "shader program");
		glDeleteShader(VertexShader);
		glDeleteShader(FragmentShader);
		glValidateProgram(id);
		GlUtil::poll_errors();
		return stn::box<shader>(id);
	}

};

