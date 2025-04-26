#include "texture.h"
#include <unordered_map>
#pragma once

namespace TextureManager {

	class TextureManager {
	public:
		std::unordered_map<std::string, ITexture*> TextureList;
		ITexture* CurrentTexture;
		Texture2D* Get2dTex(const char* File, const std::string& name);
		TextureArray* GetTexArray(array<const char*>& File, const std::string& name);
		ITexture* LoadTextureArray(array<const char*>& FileList, const std::string& name);
		ITexture* LoadTexture(const char* File, const std::string& name);
		void Apply(const std::string& name);
		void Apply( ITexture* tex);
		
	};







}