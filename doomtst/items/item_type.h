#include "../game/ecs/game_object.h"
#include <string>
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
		size_t capacity = 16;
		size_t food=0;
		size_t fertilizer = 0;
		stn::Option <blocks::block_id> blk_id = stn::None;
		bool operator==(const item_traits& other) const = default;

		static item_traits block_item(renderer::TexturePath image, blocks::block_id block_id) {
			item_traits  traits = item_traits{ .image_path = image };
			traits.blk_id=block_id;
			traits.capacity = 16;
			return traits;

		}

		static item_traits dur_item(renderer::TexturePath image, size_t initial_duribility) {
			item_traits  traits = item_traits{ .image_path = image };
			traits.duribility=initial_duribility;
			traits.capacity = 1;
			return traits;

		}

	};

	struct item_type {
		item_type() = default;
		virtual ~item_type() = default;
		virtual std::string name() const = 0;
		virtual item_traits traits(const ecs::Ecs& world) const = 0;
		
		virtual stn::Option<tool_traits> tool_info() const {
			return stn::None;
		}

	};
	using item_id = stn::typed_id<item_type>;

	//this will be removed later once we get it to work well
	template<typename T>
	concept ItemType = std::derived_from<T, item_type>&& std::is_default_constructible_v<T>;
	struct ItemTypes :ecs::resource {
		ItemTypes(const ecs::Ecs& Ecs) :world(Ecs) {

		}
		item_id from_name(const std::string_view& name) const {
			auto iter = ids.find(std::string(name));
			if (iter == ids.end()) {
				stn::throw_invalid_argument("items does not contain name {}", static_cast<std::string_view>(name));
			}
			return iter->second;
		}
		const item_type& from_id(item_id id) const {
			return *item_types[id.id];
		}
		size_t capacity_for(item_id id) const{
			return from_id(id).traits(world).capacity;
		}
		template<ItemType T>
		item_id insert() {
			stn::box<item_type> instance{ stn::construct_derived<T>() };
			std::string name = instance->name();
			if (type_id_map.contains<T>()) {
					return type_id_map.get<T>();
			}
			if (ids.contains(name)) {
				stn::throw_logic_error("items already contains a diffrent item_type with the same name as {}", instance->name());
			}
			
			item_id id = type_id_map.insert<T>().value;
			ids.emplace(instance->name(), ids.size());
			item_types.push(std::move(instance));
			return id;
		}
		
	private:
		const ecs::Ecs& world;
		stn::type_indexer<item_id> type_id_map;
		stn::array<stn::box<item_type>> item_types;
		std::unordered_map<std::string, item_id> ids;
	};


}