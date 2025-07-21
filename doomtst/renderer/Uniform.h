#pragma once
#include <variant>
#include <string>
#include "../util/SparseSet.h"
#include "../util/type_index.h"
#include "texture.h"
#include "../util/Id.h"

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
        uform_tex,
    };

    using uniform_val = std::variant<
        int,            // 0
        float,          // 1
        GLuint,         // 2  
        glm::vec2,      // 3
        glm::vec3,      // 4
        glm::vec4,      // 5
        glm::mat3,      // 6
        glm::mat4,      // 7
        Ids::Id         // texture
    >;

    struct uniform_ref {
        Ids::Id id;
        const char* name;
        uniform_ref(Ids::Id ind, const char* uniform_name) :id(ind), name(uniform_name) {
            

        }
        uniform_ref():name() {

        }
   };
    struct UniformManager {
        
        stn::array<uniform_val> uniform_list;  // Dense storage of all uniforms
        robin_hood::unordered_flat_map<const char*,size_t> name_id_map;
        void set(const char* name,const uniform_val& value) {
            uniform_list.reach(get_handle(name).id) = value;
        }
        
        
        Ids::Id get_handle(const char* name) {
            if (!name_id_map.contains(name))
            {
                name_id_map[name] = name_id_map.size();
            }
            return Ids::Id(name_id_map.at(name));
        }


        const uniform_val& get(const uniform_ref& handle) const {
            if (handle.id.id >= uniform_list.length) {
                throw std::out_of_range("Uniform handle out of range");
            }
            return uniform_list[handle.id.id];

        }
        template< typename T>
        T& get(const uniform_ref handle) {
            return std::get<T>(get(handle));
        }
    }; 
    
   inline util::pair<const char*,const char*> uparam(const char* uniform_name, const char* shader_alias) {
        return util::pair(uniform_name, shader_alias);
    };
   //self contained uniform
   struct uniform {
       uniform_val value;
       const char* name;
       uniform(const uniform_val& val,const char* uniform_name):name(uniform_name),value(val) {


       }
       template< typename T>
       T& get() {
           return std::get<T>(value);
       }
       template< typename T>
       uniform( T& val, const char* uniform_name):name(uniform_name), value(uniform_val(val)) {}

       uniform() :name() {

       }
   };
}