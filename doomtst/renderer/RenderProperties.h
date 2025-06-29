#include <GLFW/glfw3.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <stdexcept>

#pragma once

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

    // Accessor and mutator methods can be added if needed
};
class RenderMode {
public:
    std::string Name;
    RenderProperties prop;
    std::string Shader;
    Cont::array<UFunc> UniformsCalls;
    // Default constructor
    RenderMode() : Name(""), Shader(""), UniformsCalls(Cont::array<UFunc>()){}

    Cont::array<UFunc>& AddUniform(UFunc UniformCall) {
        UniformsCalls.push( UniformCall);
        return UniformsCalls;
    }
        // Constructor to initialize RenderType with parameters
        RenderMode(const std::string& name, const std::string& shader, const RenderProperties& props = {})
        : Name(name), Shader(shader), prop(props)
    {
    }
};

class RenderModeManager {
private:
    std::unordered_map<std::string, RenderMode> renderTypes;

public:
    // Add a new RenderType
    void AddType(const RenderMode& type) {
        if (Has(type.Name)) {
            throw std::runtime_error("RenderType Already Created: " + type.Name);
        }
        renderTypes[type.Name] = type;
    }

    // Check if a RenderType with the given name exists
    bool Has(const std::string& name) const {
        return renderTypes.find(name) != renderTypes.end();
    }


    // Const version of Get method
    const RenderMode& operator[](const std::string& name) const {

        if (!Has(name)) {
            throw std::runtime_error("RenderType not found: " + name);
        }
        return renderTypes.at(name);
    }

    // Clear all stored RenderTypes
    void Clear() {
        renderTypes.clear();
    }

    // Destructor
    ~RenderModeManager() = default;
};
