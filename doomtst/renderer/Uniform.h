#include <variant>
#include <string>
#include "../util/SparseSet.h"
#include "../util/type_index.h"
#include "texture.h"
#include <glm/glm.hpp>  // for vec/mat types if you use glm
#include <GLFW/glfw3.h>
namespace uniforms{
    constexpr int uform_int = 0;
    constexpr int uform_float = 1;
    constexpr int uform_gluint = 2;
    constexpr int uform_vec2 = 3;
    constexpr int uform_vec3 = 4;
    constexpr int uform_vec4 = 5;
    constexpr int uform_mat3 = 6;
    constexpr int uform_mat4 = 7;
    constexpr int uform_texarr = 8;
    constexpr int uform_tex2d = 9;
    using uniform_var = std::variant<
        int,            // 0
        float,          // 1
        GLuint,         // 2  
        glm::vec2,      // 3
        glm::vec3,      // 4
        glm::vec4,      // 5
        glm::mat3,      // 6
        glm::mat4,      // 7
        TextureArray*,  // 8
        Texture2D*      // 9
    >;

    struct uniform {
        uniform_var value;
        const char* name;
        uniform(uniform_var val, const char* uniform_name):value(val),name(uniform_name) {
            

        }
   };

    struct UniformManager {

        stn::array<uniform> uniform_list;  // Dense storage of all uniforms
        type_id::type_indexer indexer;
        template<typename T>
        void set(uniform& value) {
            auto [ind, is_new] = indexer.insert<T>();
            uniform_list.reach[ind.value] = value;
        }
        template<typename T>
        type_id::Id get_handle() {
            return indexer.insert<T>().first;
        }
        template<typename... Ts>
        stn::array<uniform> get_handles() {
            stn::array<uniform> uniforms;
            (uniforms.push(get(indexer.get<Ts>())), ...);
            return uniforms;
        }
        const uniform& get(type_id::Id handle) const {
            if (handle.value >= uniform_list.length) {
                throw std::out_of_range("Uniform handle out of range");
            }
            return uniform_list[handle.value];

        }
        
    };
}