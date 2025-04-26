#include "../renderer/ShaderManager.h"
#include "../renderer/TextureManager.h"
#include "../renderer/VertexObjectManager.h"
//Control Resource deletion and loading
struct ResourceManager
{
	vobj::Vbo* CreateVbo();
	VaoName::Vao*  CreateVao();
	vobj::Ebo*  CreateEbo();
	TextureArray* LoadTexArray(std::string Name);
	TextureArray* LoadTexArray(const dynamicarray::array<const char*> Files, std::string Name);
	Texture2D* LoadTex(std::string Name);
	Texture2D* LoadTex(const char* File, std::string Name);
	void CompileShader(const std::string& name, const char* vert, const char* frag);
	
private:
	TextureManager::TextureManager Textures;
	Shaders::ShaderManager Shaders;
	VObjMan::VObjManager Bindings;


};