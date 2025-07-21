#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <stdexcept>
#include   "shader.h"
#include "Uniform.h"
#include "ShaderManager.h"
namespace renderer {
    struct  Renderer;
}
using UFunc = std::function<void(renderer::Renderer*)>;
class RenderProperties {
public:
    bool depthTestEnabled = true;     // Enable/Disable Depth Test
    bool depthWriteEnabled = true;    // Enable/Disable Depth Write
    bool blendingEnabled = false;     // Enable/Disable Blending
    bool cullFaceEnabled = true;      // Enable/Disable Face Culling
    GLenum blendFuncSrc = GL_SRC_ALPHA;   // Source blend function
    GLenum blendFuncDst = GL_ONE_MINUS_SRC_ALPHA; // Destination blend function

    // Default constructor
    RenderProperties() = default;

    // Constructor with parameters to set initial properties
    RenderProperties(bool depthTest, bool depthWrite, bool Cull, bool Blending, GLenum BlendFunctionSrc, GLenum BlendFunctionDst)
        : depthTestEnabled(depthTest),
        depthWriteEnabled(depthWrite),
        blendingEnabled(Blending),
        cullFaceEnabled(Cull),
        blendFuncSrc(BlendFunctionSrc),
        blendFuncDst(BlendFunctionDst)
    {
    }

 
};
struct Material {

    std::string Name;
    RenderProperties prop;
    array<uniforms::uniform_ref> handles;
    shader* shade = nullptr;
    // Default constructor
    Material() : Name(""), shade() {}
        Material(const std::string& name, shader* shade, const RenderProperties& props = {}): Name(name), shade(shade), prop(props){}
};

class MaterialManager {

public:
    MaterialManager(Shaders::ShaderManager* shader_manager, uniforms::UniformManager* uniform_manager) :shader_man(shader_manager), uniform_man(uniform_manager) {
    }
    MaterialManager() {
    }
    // Check if a RenderType with the given name exists
    bool Has(const std::string& name) const {
        return string_to_id.find(name) != string_to_id.end();
    }
   
    template <typename... Args>
    void Construct(const std::string& name, const std::string& shade, const RenderProperties& props = {}, Args&&... args)
    {

        if (Has(name)) {
            throw std::runtime_error("RenderType Already Created: " + name);
        }

        auto val = Material(name, &(*shader_man)[shade], props);
        (evaluate_uniform_paramater(val, std::forward<Args>(args)), ...);
        string_to_id[val.Name] = string_to_id.size();
        materials.push(val);
        
    }

    void evaluate_uniform_paramater(Material& mat, util::pair<const char*,const char*> uniform_ref) {
        mat.handles.push(uniforms::uniform_ref( uniform_man->get_handle(uniform_ref.first), uniform_ref.second));
    }



    const size_t get_id(const std::string& name) const {

        if (!Has(name)) {
            throw std::runtime_error("RenderType not found: " + name);
        }
        return string_to_id.at(name);
    }
    const Material& get_material(const size_t id) const {

        return materials[id];
    }
    // Clear all stored RenderTypes
    void Clear() {
        string_to_id.clear();
    }

    // Destructor
    ~MaterialManager() = default;
private:
    std::unordered_map<std::string,size_t> string_to_id;
    stn::array<Material> materials;
    Shaders::ShaderManager* shader_man;
    uniforms::UniformManager* uniform_man;
};
