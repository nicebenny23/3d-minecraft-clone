#include "../game/ecs/game_object.h"
#include <string>
#include "../game/ecs/relationship.h"
#include "../renderer/uibox.h"
#include "../renderer/textrender.h"
#include "../block/block.h"
#pragma once
namespace items {
	enum class item_use_type {
		tool,
		block
	};

	struct item_traits {
		std::string item_name;
		item_use_type use_type;
		renderer::TexturePath image_path;
		float armor = 0;
		float damage = 0;
		float food=0;
		float pickaxe_speed = 0;
		stn::Option <blockname::id> blk_id = stn::None;
		bool operator==(const item_traits& other) const = default;
		item_traits(std::string_view name,item_use_type use,renderer::TexturePath image,float dmg,float protection,float food_value,float pick)
			: item_name(name),
			use_type(use),
			image_path(std::move(image)),
			armor(protection),
			damage(dmg),
			pickaxe_speed(pick),
			food(food_value),
			blk_id(stn::None) {
		}

		item_traits(std::string_view name, renderer::TexturePath image, stn::Option<blockname::id> block_id)
			: item_name(name),
			use_type(item_use_type::block),
			image_path(std::move(image)),
			armor(0),
			damage(0.0f),
			pickaxe_speed(0),
			blk_id(block_id) {
		}
	};

	struct item_type {
		virtual item_traits traits() const = 0;
	};
	using item_id = stn::typed_id<item_type>;

	//this will be removed later once we get it to work well
	template<typename T>
	concept ItemType = std::derived_from<T, item_type>&& std::is_default_constructible_v<T>;
	struct item_type_register :ecs::resource {
		item_type_register() = default;
		item_id from_name(std::string name) const {
			if (!ids.contains(name)) {
				stn::throw_invalid_argument("items does not contain name {}", name);
			}
			return ids.at(name);
		}
		item_traits from_id(item_id id) const {
			return trait_list[id.id];
		}

		template<ItemType T>
		void register_item() {
			item_traits traits = T().traits();
			if (trait_list.contains(traits)) {
				if (traits != from_id(from_name(traits.item_name))) {
					stn::throw_logic_error("items already contains a diffrent item_type with the same name as {}", traits.item_name);
				}
			}
			ids.emplace(traits.item_name, ids.size());
			trait_list.push(traits);
			

		}

	private:
		stn::array<item_traits> trait_list;
		std::unordered_map<std::string, item_id> ids;
	};


}