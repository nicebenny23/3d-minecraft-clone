#pragma once
#include <variant>
#include <string>
#include "../../util/SparseSet.h"
#include "texture.h"
#include "../../util/Id.h"
#include "../HandleMap.h"
#include "TextureManager.h"
#include <glm/glm.hpp>  // for vec/mat types if you use glm
#include <GLFW/glfw3.h>
namespace uniforms{
    enum UniformType : int {
        uform_int = 0,
        uform_float,
        uform_gluint,
        uform_vec2,
        uform_vec3,
        uform_vec4,
        uform_mat3,
        uform_mat4,
        uform_tex_2d,
		uform_tex_array
    };

    using uniform_val = std::variant<
        int,            // 0
        float,          // 1
        GLuint,         // 2  
        v2::Vec2,      // 3
        v3::Vec3,      // 4
        glm::vec4,      // 5
        glm::mat3,      // 6
        glm::mat4,      // 7
        assets::AssetHandle<Texture2D>,//8
		assets::AssetHandle<TextureArray>//9
	>;

    struct uniform_ref {
        stn::Id id;
		std::string name;
        uniform_ref(stn::Id ind, std::string uniform_name) :id(ind), name(uniform_name) {
            

        }
        uniform_ref():name() {

        }
   };

	struct UniformParam {
		std::string uniform_name;
		std::string shader_alias;
		bool operator==(const UniformParam& other) const = default;
		UniformParam(std::string uniform, std::string alias)
			: uniform_name(std::move(uniform)), shader_alias(std::move(alias)) {
		}
		UniformParam(const char* uniform, const char* alias)
			: uniform_name(std::move(uniform)), shader_alias(std::move(alias)) {
		}
	};
    struct UniformManager {
        
        handle::HandleMap< std::string,uniform_val> name_uniform_map;
        void set(std::string name,const uniform_val& value) {
            name_uniform_map.insert(name, value);
        }
        

        stn::Id handle_of(std::string name) {
            
                return name_uniform_map.reserve(name);

        }
        const uniform_val& get(const uniform_ref& handle) const {
           
            return name_uniform_map.element_of(handle.id);

        }
		uniform_ref from_param(const UniformParam& param) {

			return uniform_ref(handle_of(param.uniform_name),param.shader_alias);

		}
        template< typename T>
        T& get(const uniform_ref handle) {
            return std::get<T>(get(handle));
        }
    }; 
    

	inline UniformParam uparam(std::string uniform_name, std::string shader_alias) {
		return UniformParam{ std::move(uniform_name), std::move(shader_alias) };
	} 
	
	struct uniform {
       uniform_val value;
       std::string name;
       
       template< typename T>
       T& get() {
           return std::get<T>(value);
       }
	   
       template<typename T>
           uniform(const T& val, const char* uniform_name)
           : name(uniform_name), value(std::in_place_type<T>, val) {
       }

		uniform(const uniform_val& val, const char* uniform_name)
			: name(uniform_name), value(val) {
		}
		   UniformType current_type() {
			   return UniformType(value.index());
		   }
       uniform() :name() {

       }
   };
}