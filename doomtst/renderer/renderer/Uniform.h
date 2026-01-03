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
        const char* name;
        uniform_ref(stn::Id ind, const char* uniform_name) :id(ind), name(uniform_name) {
            

        }
        uniform_ref():name() {

        }
   };
    struct UniformManager {
        
        handle::HandleMap< std::string,uniform_val> name_uniform_map;
        void set(const char* name,const uniform_val& value) {
            name_uniform_map.insert(name, value);
        }
        

        stn::Id get_handle(const char* name) {
            
                return name_uniform_map.reserve(name);

        }
        const uniform_val& get(const uniform_ref& handle) const {
           
            return name_uniform_map.element_of(handle.id);

        }
        template< typename T>
        T& get(const uniform_ref handle) {
            return std::get<T>(get(handle));
        }
    }; 
    
    
   inline stn::pair<const char*,const char*> uparam(const char* uniform_name, const char* shader_alias) {
        return stn::pair(uniform_name, shader_alias);
    };
   //self contained uniform
   struct uniform {
       uniform_val value;
       const char* name;
       
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