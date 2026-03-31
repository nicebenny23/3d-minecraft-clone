
#pragma once
#include "Item.h"
#include "../renderer/textrender.h"
#include "../game/ecs/weak_object.h"
#include "../game/ecs/filtered_object.h"

namespace items {
	constexpr float item_size=.8f;

	struct ItemIcon:ecs::component {
		stn::Option<item_id> displayed_id;
		ecs::Constrained<ui::ui_image_component> image_component;
		ItemIcon(ecs::Constrained<ui::ui_image_component> image) :displayed_id(stn::None), image_component(image) {

		}
	};

	struct ItemCountDisplay :ecs::component {
		stn::Option<size_t> count;
		ecs::Constrained<ui::text_component> text_component;
		ItemCountDisplay(ecs::Constrained<ui::text_component> text) :count(stn::None), text_component(text) {

		}
	};
	struct ItemProgressDisplay :ecs::component {
		stn::Option<double> value;
		geo::Box2d full_box;
		ecs::Constrained<ui::ui_image_component> bar;
		ItemProgressDisplay(ecs::Constrained<ui::ui_image_component> image_bar, geo::Box2d box) :value(0), bar(image_bar), full_box(box){

		}
	};

	struct ItemUiSpawner{
		ItemUiSpawner(){

		}
		void apply(ecs::obj& entity) const{
			ecs::obj image= entity.spawn_child< ui::ui_image_spawner>(geo::Box2d::origin_centered(v2::Vec2(item_size, item_size)), 1);
			ecs::obj text= entity.spawn_child<ui::ui_text_spawner>(geo::Box2d::Box2d(v2::Vec2(.4f,.4f),v2::Vec2(.4f,.4f)), 1,colors::White);
			entity.set_emplace_component<ItemIcon>(ecs::Constrained<ui::ui_image_component>(image));
			entity.set_emplace_component<ItemCountDisplay>(ecs::Constrained<ui::text_component>(text));
				geo::Box2d max_size(v2::Vec2(0, -.7f), v2::Vec2(item_size, item_size / 4));
				ecs::obj bar= entity.spawn_child< ui::ui_image_spawner>(renderer::TexturePath("images\\default.png"), max_size, 1, colors::Green);
				entity.set_emplace_component<ItemProgressDisplay>(bar,max_size);
		}
	};

	struct ItemDuribilityBarSetter :ecs::System {
		void run(ecs::Ecs& world) {
			for (auto&& [icon] : ecs::View<ItemProgressDisplay>(world)) {
				if (!icon.value) {
					icon.bar.get_component<ui::UiEnabled>().disable();
				}
				else {
					icon.bar.get_component<ui::UiEnabled>().enable();

					double dur_progress = icon.value.unwrap();
					geo::Box2d& box = icon.bar.get_component<ui::UiBounds>().local;
					box.scale.x = icon.full_box.scale.x * dur_progress;
					box.center.x = stn::lerp(icon.full_box.min().x, icon.full_box.center.x, dur_progress);
				}
			}
		}
	};
	struct ItemIconSetter :ecs::System {
		void run(ecs::Ecs& world) {
			for (auto&& [icon] : ecs::View<ItemIcon>(world)) {
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

			for (auto&& [count] : ecs::View< ItemCountDisplay>(world)) {
				if (!count.count||count.count.unwrap()<=1) {
					count.text_component.get_component<ui::UiEnabled>().disable();
					continue;
				}
				count.text_component.get_component<ui::UiEnabled>().enable();
				count.text_component.get_component<ui::text_component>().format("{}",count.count.unwrap());
			}
		}
	};

	struct ItemUiPlugin :Core::Plugin {
		void build(Core::App& app) {
			app.insert_plugin<ui::UiImagePlugin>();
			app.insert_plugin<ui::UiTextPlugin>();
			app.emplace_system<ItemIconSetter>();
			app.emplace_system<ItemDuribilityBarSetter>();
			app.emplace_system<ItemCountSetter>();
		}
	};
}