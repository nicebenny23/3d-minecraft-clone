#include "texture.h"
#include "../util/Id.h"
#include <unordered_map>
#include "HandleMap.h"
#pragma once

namespace TextureManager {

	class TextureManager {
	public:
		TextureManager() {


		}
		handle::HandleMap<ITexture*> texture_list;
		Ids::Id get_handle(std::string Name) {
			return texture_list.get_handle(Name);
		}
		ITexture& get_texture(Ids::Id handle) {
			return *texture_list.get_elem(handle);
		}
		Ids::Id LoadTextureArray(array<const char*>& FileList, const std::string& name);
		Ids::Id LoadTexture(const char* File, const std::string& name);
		
		
	};







}