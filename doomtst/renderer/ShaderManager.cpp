#include "ShaderManager.h"

void Shaders::ShaderManager::Compile(const std::string& name, const char* vert, const char* frag)
{

        shader* CompiledShader = new shader(name.c_str(), vert, frag);
        if (shaderlist[name] != nullptr)
        {
            throw std::logic_error("shaders are unable to have the same Name");
        }
        shaderlist[name] = CompiledShader;

    }

void Shaders::ShaderManager::Bind(const std::string Name)
{
    shader* ShaderWithName = shaderlist[Name];
    if (ShaderWithName==nullptr)
    {
        throw std::invalid_argument("Shader " + Name + " Does Not Exist");
    }

    ShaderWithName->attach();
    BoundShader = ShaderWithName;
}
