
#pragma once
#include "Item.h"

namespace items {
	constexpr float item_size=.8f;

	struct ItemUiComponent:ecs::component {
		ecs::obj text_component;
		ecs::obj image_component;
		ItemUiComponent(ecs::obj image, ecs::obj text) :text_component(text), image_component(image){

		}
	};
	struct set_ui_count {
		set_ui_count(ecs::obj elem, size_t cnt) :element(elem), count(cnt) {

		}
		size_t count;
		ecs::obj element;
	};

	struct ItemUiSpawner:ecs::Recipe {
		item_entry base;
		ItemUiSpawner(item_entry spawn_entry) :base(spawn_entry){

		}
		void apply(ecs::obj& entity) {
			ItemSpawner(base).apply(entity);
			item_traits traits = entity.world().ensure_resource<item_type_register>().from_id(base.id);
			ecs::obj image= entity.spawn_child< ui::ui_image_spawner>(traits.image_path,geo::Box2d::origin_centered(v2::Vec2(item_size, item_size)), 20);
			ecs::obj text= entity.spawn_child<ui::ui_text_spawner>(geo::Box2d::Box2d(v2::Vec2(.5f,.5f),v2::Vec2(.5f,.5f)), 0);
			ui::ui_spawner(geo::Box2d::origin_centered(v2::Vec2(item_size, item_size)), 20).apply(entity);
			entity.ensure_component<ItemUiComponent>(image, text);
		}
	};

	struct item_ui_updater :ecs::System {
		void run(ecs::Ecs& world) {
			for (auto&& [ui_element,item] : ecs::View<ItemUiComponent,item_stack>(world)) {
				world.write_command(set_ui_count(ui_element.owner(), item.count()));
			}
			for (set_ui_count count:world.read_commands<set_ui_count>()) {
				count.element.get_component<ItemUiComponent>().text_component.get_component<ui::text_component>().format("{}", count.count);
			}
		}
	};
	struct ItemUiPlugin :Core::Plugin {
		void build(Core::App& app) {
			app.emplace_system<item_ui_updater>();
		}
	};
}