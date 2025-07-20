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
class Material {
public:
    std::string Name;
    RenderProperties prop;
    array<type_id::Id> handles;
    shader* shade = nullptr;
    // Default constructor
    Material() : Name(""), shade(){}

   
    
        // Constructor to initialize RenderType with parameters
        Material(const std::string& name, shader* shade, const RenderProperties& props = {})
        : Name(name), shade(shade), prop(props)
    {
    }
};

class MaterialManager {
private:
    std::unordered_map<std::string, Material> renderTypes;
    Shaders::ShaderManager* shader_man;
    uniforms::UniformManager* uniform_man;
public:
    
    // Check if a RenderType with the given name exists
    bool Has(const std::string& name) const {
        return renderTypes.find(name) != renderTypes.end();
    }
    template<typename ...uniforms>
    void Construct(const std::string& name, const std::string& shade, const RenderProperties& props = {})
    {
        if (Has(name)) {
            throw std::runtime_error("RenderType Already Created: " + name);
        }

        auto val=(Material(name, &(*shader_man)[shade], props));
        val.handles= uniform_man->get_handles<uniforms...>();
        renderTypes[type.Name] = type;
    }
    // Const version of Get method

    const Material& operator[](const std::string& name) const {

        if (!Has(name)) {
            throw std::runtime_error("RenderType not found: " + name);
        }
        return renderTypes.at(name);
    }
    MaterialManager(Shaders::ShaderManager* shader_manager,uniforms::UniformManager* uniform_manager) :shader_man(shader_manager),uniform_man(uniform_manager){
    }
    MaterialManager() {
    }
    // Clear all stored RenderTypes
    void Clear() {
        renderTypes.clear();
    }

    // Destructor
    ~MaterialManager() = default;
};
