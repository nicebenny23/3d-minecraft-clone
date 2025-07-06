#pragma once
#include "TextureManager.h"
#include "../util/stbiload.h"




Texture2D* TextureManager::TextureManager::Get2dTex(const char* File, const std::string& name)
{
    ITexture* LoadedTex = TextureList[name];
    if (LoadedTex == nullptr)
    {
        LoadedTex = LoadTexture(File, name);
    }
    if (LoadedTex->type != Tex2d)
    {
        throw std::logic_error("Wrong texture type");
    }
    return (Texture2D*)(LoadedTex);
}

TextureArray* TextureManager::TextureManager::GetTexArray(array<const char*>& File, const std::string& name)
{
    ITexture* LoadedTex = TextureList[name];
    if (LoadedTex == nullptr)
    {
        LoadedTex= LoadTextureArray(File, name);
    }
    if (LoadedTex->type != TexArray)
    {
        throw std::logic_error("Wrong texture type");
    }
    return (TextureArray*)(LoadedTex);
}
ITexture* TextureManager::TextureManager::LoadTextureArray(array<const char*>& File, const std::string& name)
{
    TextureArray* Tex = new TextureArray(File, name.c_str());
    TextureList[name] = Tex;
   
    return Tex;
}
ITexture* TextureManager::TextureManager::LoadTexture(const char* File, const std::string& name)
{
   
    Texture2D* Tex = new Texture2D(File,name.c_str());
    TextureList[name] = Tex;
   
    return Tex;
}



