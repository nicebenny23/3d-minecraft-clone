#pragma once
#include "ResourceManager.h"
using namespace vobj;
using namespace VaoName;
vobj::Vbo* ResourceManager::CreateVbo()
{
	Vbo*New= new Vbo();
	Bindings.Create(New);
	return New;
}

VaoName::Vao* ResourceManager::CreateVao()
{
	Vao* New = new Vao();
	Bindings.Create(New);
	return New;
}

vobj::Ebo* ResourceManager::CreateEbo()
{
	Ebo* New = new Ebo();
	Bindings.Create(New);
	return New;
}

TextureArray* ResourceManager::LoadTexArray(std::string Name)
{
	return nullptr;
	// return Textures.(Name);
}

TextureArray* ResourceManager::LoadTexArray(const Cont::array<const char*> Files, std::string Name)
{
	return nullptr;
}

Texture2D* ResourceManager::LoadTex(std::string Name)
{
	return nullptr;
}

Texture2D* ResourceManager::LoadTex(const char* File, std::string Name)
{
	return nullptr;
}

