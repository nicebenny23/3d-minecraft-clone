
#pragma once
#include "Item.h"

namespace items {
	constexpr float item_size=.8f;

	struct ItemIcon:ecs::component {
		stn::Option<item_id> displayed_id;
		ecs::obj image_component;
		ItemIcon(ecs::obj image) :displayed_id(stn::None), image_component(image) {

		}
	};

	struct ItemCountDisplay :ecs::component {
		stn::Option<size_t> count;
		ecs::obj text_component;
		ItemCountDisplay(ecs::obj text) :count(stn::None), text_component(text) {

		}
	};

	struct ItemUiSpawner:ecs::Recipe {
	
		ItemUiSpawner( ){

		}
		void apply(ecs::obj& entity) {
			ecs::obj image= entity.spawn_child< ui::ui_image_spawner>(geo::Box2d::origin_centered(v2::Vec2(item_size, item_size)), 20);
			ecs::obj text= entity.spawn_child<ui::ui_text_spawner>(geo::Box2d::Box2d(v2::Vec2(.5f,.5f),v2::Vec2(.5f,.5f)), 30);
			entity.ensure_component<ItemIcon>(image);
			entity.ensure_component<ItemCountDisplay>(text);
			ui::ui_spawner(geo::Box2d::origin_centered(v2::Vec2(item_size, item_size)), 20).apply(entity);
		}
	};
	struct DisplayedItemSpawner :ecs::Recipe {
		item_entry base;
		DisplayedItemSpawner(item_entry spawn_entry) :base(spawn_entry) {

		}
		void apply(ecs::obj& entity) {
			ItemSpawner(base).apply(entity);
			ItemUiSpawner().apply(entity);
		}
	};
	struct ItemIconSetter :ecs::System {
		void run(ecs::Ecs& world) {
			
			for (auto&& [icon] : ecs::View<ItemIcon>(world)) {

				if (!icon.displayed_id) {
					icon.image_component.get_component<ui::UiEnabled>().disable();
					continue;
				}
				item_traits traits = world.get_resource<item_types>().expect("item_type_registry must exist for items to exist")
				.from_id(icon.displayed_id.unwrap());
				icon.image_component.get_component<ui::UiEnabled>().enable();
				icon.image_component.get_component<ui::ui_image_component>().set_image(traits.image_path);
			}
		}
	};
	struct ItemCountSetter :ecs::System {
		void run(ecs::Ecs& world) {

			for (auto&& [count] : ecs::View<ItemCountDisplay>(world)) {
				if (!count.count) {
					count.text_component.get_component<ui::UiEnabled>().disable();
					continue;
				}
				count.text_component.get_component<ui::UiEnabled>().enable();
				count.text_component.get_component<ui::text_component>().format("{}",count.count.unwrap());
			}
		}
	};
	struct SyncItemStackUi:ecs::System {

		void run(ecs::Ecs& world) {

			for (auto&& [icon, item] : ecs::View<ItemIcon, item_stack>(world)) {
				icon.displayed_id = item.contained_id();
			}
	
			for (auto&& [count, item] : ecs::View<ItemCountDisplay, item_stack>(world)) {
				count.count = item.count();
			}
		}
	};
	struct ItemUiPlugin :Core::Plugin {
		void build(Core::App& app) {
			app.emplace_system<ItemIconSetter>();

			app.emplace_system<ItemCountSetter>();
			app.emplace_system<SyncItemStackUi>();
		}
	};
}