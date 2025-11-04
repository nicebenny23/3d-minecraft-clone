#pragma once
#include "ShaderManager.h"

void renderer::Shaders::Compile(const std::string& name, const char* vert, const char* frag)
{
        shader_map.insert(name,shader(name, vert, frag));
}

renderer::shader_id renderer::Shaders::handle_of(const std::string& Name)
{
    return shader_map.reserve(Name);
}

shader& renderer::Shaders::shader_of(shader_id handle)
{

    return shader_map[handle].unwrap();
}

shader& renderer::Shaders::shader_of(const std::string& Name)
{
    return shader_map.element_of(Name);
}
