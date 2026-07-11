#pragma once
#include <variant>
#include <string>
#include "texture.h"
#include "../../util/Id.h"
#include "../HandleMap.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "texture.h"
namespace renderer {

	using uniform_value = std::variant<
		GLint,            
		float,          
		GLuint,           
		v2::Vec2,      
		v3::Vec3,      
		glm::vec4,      
		glm::mat3,      
		glm::mat4,      
		assets::AssetHandle<Texture2D>,
		assets::AssetHandle<TextureArray>
	>;



	struct UniformRefrence {
		std::string uniform_name;
		std::string shader_alias;
		bool operator==(const UniformRefrence& other) const = default;
		UniformRefrence(std::string_view uniform, std::string_view alias)
			: uniform_name(uniform), shader_alias(alias) {
		}
		UniformRefrence(const char* uniform, const char* alias)
			: uniform_name(std::move(uniform)), shader_alias(std::move(alias)) {
		}
	};


	using UniformId = stn::typed_id<uniform_value>;
	struct UniformRegistry {

		handle::HandleMap< std::string, uniform_value, UniformId> name_uniform_map;
		void set(std::string name, const uniform_value& value) {
			name_uniform_map.insert(name, value);
		}

		const uniform_value& get(const UniformRefrence& handle) const {

			return name_uniform_map.element_of(handle.uniform_name);

		}

	};


	struct uniform {
		uniform_value value;
		std::string name;

		template< typename T>
		T& get() {
			return std::get<T>(value);
		}

		template< typename T>
		const T& get() const {
			return std::get<T>(value);
		}

		template<typename T>
		uniform(const T& val, const char* uniform_name)
			: name(uniform_name), value(std::in_place_type<T>, val) {
		}

		uniform(const uniform_value& val, const std::string& uniform_name)
			: name(uniform_name), value(val) {
		}
		uniform() :name() {

		}
	};
}