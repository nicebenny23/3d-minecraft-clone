#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <stdexcept>
#include   "shader.h"

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
class Base_Material {
public:
    std::string Name;
    RenderProperties prop;

    shader* shade = nullptr;
    Cont::array<UFunc> UniformsCalls;
    // Default constructor
    Base_Material() : Name(""), shade(), UniformsCalls(Cont::array<UFunc>()){}

    Base_Material& AddUniform(UFunc UniformCall) {
        UniformsCalls.push( UniformCall);
        return *this;
    }
        // Constructor to initialize RenderType with parameters
        Base_Material(const std::string& name, shader* shade, const RenderProperties& props = {})
        : Name(name), shade(shade), prop(props)
    {
    }
};

class RenderModeManager {
private:
    std::unordered_map<std::string, Base_Material> renderTypes;
    Shaders::ShaderManager* shader_man;
public:
    // Add a new RenderType
    void AddType(const Base_Material& type) {
        if (Has(type.Name)) {
            throw std::runtime_error("RenderType Already Created: " + type.Name);
        }
        renderTypes[type.Name] = type;
    }
    // Check if a RenderType with the given name exists
    bool Has(const std::string& name) const {
        return renderTypes.find(name) != renderTypes.end();
    }
    Base_Material Construct(const std::string& name, const std::string& shade, const RenderProperties& props = {})
    {
        return  Base_Material(name, &(*shader_man)[shade], props);
    }
    // Const version of Get method

    const Base_Material& operator[](const std::string& name) const {

        if (!Has(name)) {
            throw std::runtime_error("RenderType not found: " + name);
        }
        return renderTypes.at(name);
    }
    RenderModeManager(Shaders::ShaderManager* shader_manager) :shader_man(shader_manager) {


    }
    RenderModeManager() {


    }
    // Clear all stored RenderTypes
    void Clear() {
        renderTypes.clear();
    }

    // Destructor
    ~RenderModeManager() = default;
};
