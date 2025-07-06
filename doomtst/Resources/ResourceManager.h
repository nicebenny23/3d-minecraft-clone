#pragma once
#include "../renderer/ShaderManager.h"
#include "../renderer/TextureManager.h"
#include "../renderer/VertexObjectManager.h"
//Control Resource deletion and loading
struct ResourceManager
{

	buffer_object::Vbo* CreateVbo();
	VaoName::Vao*  CreateVao();
	buffer_object::Ebo*  CreateEbo();
	TextureArray* LoadTexArray(std::string Name);
	TextureArray* LoadTexArray(const Cont::array<const char*> Files, std::string Name);
	Texture2D* LoadTex(std::string Name);
	Texture2D* LoadTex(const char* File, std::string Name);
	void CompileShader(const std::string& name, const char* vert, const char* frag);
	
private:
	TextureManager::TextureManager Textures;
	Shaders::ShaderManager Shaders;
	VObjMan::VObjManager Bindings;


};