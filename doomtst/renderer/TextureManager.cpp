#pragma once
#include "TextureManager.h"
#include "../util/stbiload.h"




Ids::Id TextureManager::TextureManager::LoadTextureArray(array<const char*>& File, const std::string& name)
{
    if (!texture_list.Contains(name))
    {

        TextureArray* Tex = new TextureArray(File, name.c_str());
        texture_list.set(name, (ITexture*)(Tex));
    }
    return texture_list.get_handle(name);

}
Ids::Id TextureManager::TextureManager::LoadTexture(const char* File, const std::string& name)
{
   
    if (!texture_list.Contains(name))
    {

        Texture2D* Tex = new Texture2D(File, name.c_str());
        texture_list.set(name, (ITexture*)(Tex));
    }
    return texture_list.get_handle(name);
}



