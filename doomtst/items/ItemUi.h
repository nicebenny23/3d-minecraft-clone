
#pragma once
#include "Item.h"
#include "../renderer/textrender.h"
#include "../game/ecs/weak_object.h"
#include "../game/ecs/filtered_object.h"

namespace items {
	constexpr float item_size=.75f;

	struct ItemIcon:ecs::component {
		stn::Option<item_id> displayed_id;
		ecs::Constrained<ui::UiImage> image_component;
		ItemIcon(ecs::Constrained<ui::UiImage> image) :displayed_id(stn::None), image_component(image) {

		}
	};

	struct ItemCountDisplay :ecs::component {
		stn::Option<size_t> count;
		ecs::Constrained<ui::Text> Text;
		ItemCountDisplay(ecs::Constrained<ui::Text> text) :count(stn::None), Text(text) {

		}
	};
	struct ItemProgressDisplay :ecs::component {
		stn::Option<double> value;

		ecs::Constrained<ui::UiImage> under_bar;
		ecs::Constrained<ui::UiImage> bar;
		ItemProgressDisplay(ecs::Constrained<ui::UiImage> image_bar, ecs::Constrained<ui::UiImage> under_bar) :value(stn::None), bar(image_bar), under_bar(under_bar){

		}
	};

	inline void ItemUiSpawner(ecs::obj& entity){
			ecs::obj image= entity.spawn_child< ui::UiImageSpawner>(geo::Box2d::origin_centered(v2::Vec2(item_size, item_size)), 1);
			ecs::obj text= entity.spawn_child<ui::UiTextSpawner>(geo::Box2d::Box2d(v2::Vec2(.2f,.2f),v2::Vec2(.4f,.4f)), 1,colors::White);
			entity.set_emplace_component<ItemIcon>(ecs::Constrained<ui::UiImage>(image));
			entity.set_emplace_component<ItemCountDisplay>(ecs::Constrained<ui::Text>(text));
				geo::Box2d max_size(v2::Vec2(0, -.4f), v2::Vec2(item_size, item_size / 8));
				ecs::obj bar= entity.spawn_child< ui::UiImageSpawner>(renderer::TexturePath("images\\default.png"), max_size, 1, colors::Green);
				ecs::obj under_bar = entity.spawn_child< ui::UiImageSpawner>(renderer::TexturePath("images\\default.png"), max_size, 0, colors::Black);
				entity.set_emplace_component<ItemProgressDisplay>(bar, under_bar);
	}

	struct ItemDuribilityBarSetter :ecs::System {
		void run(ecs::Ecs& world) {
			for (auto&& [icon] : ecs::View<ItemProgressDisplay>(world)) {
				if (!icon.value||icon.value==1) {

					icon.under_bar.get_component<ui::UiEnabled>().disable();
					icon.bar.get_component<ui::UiEnabled>().disable();
				}
				else {
					icon.bar.get_component<ui::UiEnabled>().enable();
					icon.under_bar.get_component<ui::UiEnabled>().enable();

					double dur_progress = icon.value.unwrap();
					geo::Box2d& box = icon.bar.get_component<ui::UiBounds>().local;
					geo::Box2d under_bar_box= icon.under_bar.get_component<ui::UiBounds>().local;

					geo::Box2d& final_box = icon.bar.get_component<ui::ComputedStyle>().final_size;
					icon.bar.get_component<ui::UiImage>().current_color = colors::lerp(colors::Red, colors::Green, dur_progress);
					box.scale.x = under_bar_box.scale.x * dur_progress;
					box.center.x = stn::lerp(under_bar_box.min().x, under_bar_box.center.x, dur_progress);
					double min = final_box.min().x;
					int l = 2;
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
				item_traits traits = world.get_resource<ItemTypes>()
				.from_id(icon.displayed_id.unwrap()).traits(world);
				icon.image_component.get_component<ui::UiEnabled>().enable();
				icon.image_component.get_component<ui::UiImage>().set_image(traits.image_path);
			}
		}
	};
	struct ItemCountSetter :ecs::System {
		void run(ecs::Ecs& world) {

			for (auto&& [count] : ecs::View< ItemCountDisplay>(world)) {
				if (!count.count||count.count.unwrap()<=1) {
					count.Text.get_component<ui::UiEnabled>().disable();
					continue;
				}
				count.Text.get_component<ui::UiEnabled>().enable();
				count.Text.get_component<ui::Text>().format("{}",count.count.unwrap());
			}
		}
	};

	struct ItemUiPlugin {
		void operator()(core::App& app) {
			app.insert_plugin(ui::UiImagePlugin());
			app.insert_plugin(ui::UiTextPlugin());
			app.emplace_system<ItemIconSetter>();
			app.emplace_system<ItemDuribilityBarSetter>();
			app.emplace_system<ItemCountSetter>();
		}
	};
}