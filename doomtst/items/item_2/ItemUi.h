
#pragma once
#include "Item.h"

namespace items {
	constexpr float item_size=.02f;

	struct ItemUiComponent:ecs::component {
		ecs::obj text_component;
		ecs::obj image_component;
		ItemUiComponent(ecs::obj image, ecs::obj text, size_t initial_count) :text_component(text), image_component(image){

		}
	};
	struct set_ui_count {
		set_ui_count(ecs::obj elem, size_t cnt) :element(elem), count(cnt) {

		}
		size_t count;
		ecs::obj element;
	};

	struct ItemUiSpawner {
		item_id id;
		size_t base_amount;

		void apply(ecs::obj& entity) {
			item_traits traits = entity.world().ensure_resource<item_type_register>().from_id(id);
			ecs::obj image= ecs::spawn_child< ui::ui_image_spawner>(entity,traits.image_path,geo::Box2d::origin_centered(v2::Vec2(item_size, item_size)), 20);
			ecs::obj text=ecs::spawn_child< ui::ui_text_spawner>(entity,geo::Box2d::origin_centered(v2::Vec2(item_size, item_size)), 20);
			ui::ui_spawner(geo::Box2d::origin_centered(v2::Vec2(item_size, item_size)), 20).apply(entity);
			entity.ensure_component<ItemUiComponent>(image, text, 0);
		}
	};

	struct item_ui_updater :ecs::System {
		void run(ecs::Ecs& world) {
			for (set_ui_count count:world.read_commands< set_ui_count>()) {
				count.element.get_component<ItemUiComponent>().text_component.get_component<ui::text_component>().format("{}", count.count);
			}
		}

	};
}