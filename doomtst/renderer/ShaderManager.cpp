#pragma once
#include "ShaderManager.h"

void Shaders::ShaderManager::Compile(const std::string& name, const char* vert, const char* frag)
{
        size_t handle = get_handle(name).id;
        if (handle<shaderlist.length)
        {
            throw std::logic_error("shaders are unable to have the same Name");
        }
        shaderlist.push(shader(name.c_str(), vert, frag));

    }

Ids::Id Shaders::ShaderManager::get_handle(std::string Name)
{
  
    if (!string_to_id.contains(Name))
    {
        string_to_id[Name] = string_to_id.size();
    }
    return Ids::Id(string_to_id[Name]);
}

shader& Shaders::ShaderManager::get_shader(Ids::Id handle)
{
    if (shaderlist.length<handle.id)
    {
        throw std::out_of_range("Requested Shader does not exist");
    }
    return shaderlist[handle.id];
}
