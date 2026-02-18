
#pragma once
#include "Item.h"
#include "../renderer/textrender.h"
#include "../game/ecs/weak_object.h"

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

	struct ItemUiSpawner{
	
		ItemUiSpawner( ){

		}
		void apply(ecs::obj& entity) {
			ecs::obj image= entity.spawn_child< ui::ui_image_spawner>(geo::Box2d::origin_centered(v2::Vec2(item_size, item_size)), 1);
			ecs::obj text= entity.spawn_child<ui::ui_text_spawner>(geo::Box2d::Box2d(v2::Vec2(.5f,.5f),v2::Vec2(.5f,.5f)), 1);
			entity.set_emplace_component<ItemIcon>(image);
			entity.set_emplace_component<ItemCountDisplay>(text);	
		}
	};
	struct DisplayItem :ecs::component {
		stn::Option<item_entry> display;
	};
	struct DisplayedItemSpawner {

		DisplayedItemSpawner() {
		}
		void apply(ecs::obj& entity) {

			entity.add_component<DisplayItem>();
			ItemUiSpawner().apply(entity);
		}
	};

	
	struct ItemIconSetter :ecs::System {
		void run(ecs::Ecs& world) {
			
			for (auto&& [icon] : ecs::View<ecs::With<ItemIcon>>(world)) {

				if (!icon.displayed_id) {
					icon.image_component.get_component<ui::UiEnabled>().disable();
					continue;
				}
				item_traits traits = world.get_resource<item_types>()
				.from_id(icon.displayed_id.unwrap());
				icon.image_component.get_component<ui::UiEnabled>().enable();
				icon.image_component.get_component<ui::ui_image_component>().set_image(traits.image_path);
			}
		}
	};
	struct ItemCountSetter :ecs::System {
		void run(ecs::Ecs& world) {

			for (auto&& [count] : ecs::View<ecs::With<ItemCountDisplay>>(world)) {
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
			for (auto&& [display] : ecs::View<ecs::With<DisplayItem>>(world)) {
				if (display.display&&display.display.unwrap().count==0) {
					display.display=stn::None;
				}
			}
			for (auto&& [icon, display] : ecs::View<ecs::With<ItemIcon>,ecs::With< DisplayItem>>(world)) {
				if (display.display) {
					icon.displayed_id = display.display.unwrap().id;
				}
				else {
					icon.image_component.get_component<ui::UiEnabled>().disable();
				}
			}
	
			for (auto&& [count, display] : ecs::View<ecs::With<ItemCountDisplay>, ecs::With<DisplayItem>>(world)) {
				if (display.display) {
					count.count= display.display.unwrap().count;
				}
				else {
					count.text_component.get_component<ui::UiEnabled>().disable();
				}
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