#include "../game/ecs/game_object.h"
#include <string>
#include "../util/String.h"
#include "../block/block_registry.h"
#include "../renderer/renderer.h"
#pragma once
namespace items {
	enum class item_use_type {
		use,
		block
	};

	struct tool_traits {
		float damage;
		float pickaxe_speed;
		bool operator==(const tool_traits& other) const = default;
	};
	struct item_traits {
		std::string item_name;
		stn::Option<size_t> duribility;
		renderer::TexturePath image_path;
		stn::Option<tool_traits> tool;
		float armor = 0;
		float food=0;
		stn::Option <blocks::block_id> blk_id = stn::None;
		bool operator==(const item_traits& other) const = default;
		item_traits(std::string_view name, renderer::TexturePath image, size_t initial_duribility, tool_traits traits, float protection, float food_value)
			: item_name(name),
			duribility(initial_duribility),
			image_path(std::move(image)),
			armor(protection),
			tool(traits),
			food(food_value),
			blk_id(stn::None) {
		}
		item_traits(std::string_view name, renderer::TexturePath image,blocks::block_id blk_id)
			: item_name(name),
			duribility(),
			image_path(std::move(image)),
			armor(0),
			blk_id(blk_id) {
		}
		item_traits(std::string_view name, renderer::TexturePath image)
			: item_name(name),
			duribility(),
			image_path(std::move(image)),
			armor(0),
			blk_id(stn::None) {
		}
	};

	struct item_type {
		virtual item_traits traits(const ecs::Ecs& world) const = 0;
	};
	using item_id = stn::typed_id<item_type>;

	//this will be removed later once we get it to work well
	template<typename T>
	concept ItemType = std::derived_from<T, item_type>&& std::is_default_constructible_v<T>;
	struct item_types :ecs::resource {
		item_types(const ecs::Ecs& Ecs) :world(Ecs) {

		}
		item_id from_name(const std::string_view& name) const {
			auto iter = ids.find(name);
			if (iter == ids.end()) {
				stn::throw_invalid_argument("items does not contain name {}", static_cast<std::string_view>(name));
			}
			return iter->second;
		}
		template<ItemType T>
		item_id from_type() const {
			return type_id_map.at<T>().expect("item should be initialized");
		}
		item_traits from_id(item_id id) const {
			return trait_list[id.id];
		}
		size_t capacity_for(item_id id) const{
			return 64;
		}
		template<ItemType T>
		void register_item() {
			item_traits traits = T().traits(world);
			if (trait_list.contains(traits)) {
				if (traits != from_id(from_name(traits.item_name))) {
					stn::throw_logic_error("items already contains a diffrent item_type with the same name as {}", traits.item_name);
				}
			}
			ids.emplace(traits.item_name, ids.size());
			trait_list.push(traits);
			type_id_map.set<T>(item_id(trait_list.last_index()));
		}
		
	private:
		const ecs::Ecs& world;
		stn::type_map<item_id> type_id_map;
		stn::array<item_traits> trait_list;
		std::unordered_map<std::string, item_id, stn::StringHasher,std::equal_to<>> ids;
	};


}