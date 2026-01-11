#include "../../game/ecs/game_object.h"
#include <string>
#include "../../game/ecs/relationship.h"
#include "../../renderer/uibox.h"
#include "../../renderer/textrender.h"

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
		bool operator==(const item_traits& other) const = default;
	};

	struct item_type {
		virtual const item_traits& traits() const = 0;
	};
	using item_id = stn::typed_id<item_type>;

	//this will be removed later once we get it to work well
	template<typename T>
	concept ItemType = std::derived_from<T, item_type>&&std::is_default_constructible_v<T>;
	struct item_type_register :ecs::resource {
		item_type_register() = default;
		item_id from_name(std::string name) const{
			if (!ids.contains(name)) {
				stn::throw_invalid_argument("items does not contain name {}", name);
			}
		}
		item_traits from_id(item_id id) const{
			return trait_list[id.id];
		}

		template<ItemType T>
		void register_item() {
			item_traits traits = T().traits();
			if (trait_list.contains(traits)) {
				if (traits!=from_id(from_name(traits.item_name))) {
					stn::throw_logic_error("items already contains a diffrent item_type with the same name as {}", traits.item_name);
				}
			}
			if (ids) {
				ids.emplace(traits.item_name, ids.size());
				trait_list.push(traits);
			}

		}

	private:
		stn::array<item_traits> trait_list;
		std::unordered_map<std::string, item_id> ids;
	};


}