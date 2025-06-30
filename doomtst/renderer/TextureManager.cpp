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
    ITexture* CurrTex = CurrentTexture;
    TextureArray* Tex = new TextureArray(File, name.c_str());
    TextureList[name] = Tex;
    if (CurrentTexture!=nullptr)
    {
        CurrTex->apply();
    }
    return Tex;
}
ITexture* TextureManager::TextureManager::LoadTexture(const char* File, const std::string& name)
{
    ITexture* CurrTex = CurrentTexture;
    Texture2D* Tex = new Texture2D(File,name.c_str());
    TextureList[name] = Tex;
    if (CurrentTexture != nullptr)
    {
        CurrTex->apply();
    }
    return Tex;
}

void TextureManager::TextureManager::Apply(const std::string& name)
{
    ITexture* ToApply = TextureList[name];
    if (ToApply==nullptr)
    {
        throw std::logic_error("Texture " + name + " Does Not Exist So it Cant Be Applied");
    }
    ToApply->apply();
    CurrentTexture = ToApply;
}

void TextureManager::TextureManager::Apply( ITexture* tex)
{
    if (tex == nullptr)
    {
        throw std::logic_error("Cannot Apply texture Nullptr");
    }
    tex->apply();
    CurrentTexture = tex;
}

