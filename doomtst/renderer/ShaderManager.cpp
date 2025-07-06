#pragma once
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
