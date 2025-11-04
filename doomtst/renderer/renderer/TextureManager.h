#include "texture.h"
#include "../../util/Id.h"
#include "../../util/stbiload.h"
#include <unordered_map>
#include "../HandleMap.h"
#pragma once

namespace renderer {

	struct texture_id_tag;

	using texture_id = stn::default_id<texture_id_tag>;
	class TextureManager {
	public:
		TextureManager() {


		}
		handle::HandleMap<std::string,ITexture*, stn::typed_id<texture_id_tag>> texture_list;
		texture_id get_handle(std::string Name) {
			return texture_list.id_of(Name);
		}
		ITexture& get_texture(texture_id handle) {
			return *texture_list[handle.as_id()].unwrap();
		}
		texture_id LoadTextureArray(array<const char*>& File, const std::string& name)
		{
			if (!texture_list.generated(name))
			{

				TextureArray* Tex = new TextureArray(File, name.c_str());
				texture_list.insert(name,(ITexture*)(Tex));
			}
			return texture_list.id_of(name);

		}
		texture_id LoadTexture(const char* File, const std::string& name)
		{

			if (!texture_list.generated(name))
			{

				Texture2D* Tex = new Texture2D(File, name.c_str());
				texture_list.insert(name, (ITexture*)(Tex));
			}
			return texture_list.id_of(name);
		}

		
	};







}