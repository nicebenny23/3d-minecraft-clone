#include "../game/ecs/game_object.h"
#include "../game/collision.h"
#include "../player/player.h"
#include "../math/interval.h"
#include "../game/casts.h"
#include <utility>
#include "../items/menu.h"
#pragma once
namespace player {
	struct CursorHit {
		ecs::obj hit;
	};
	struct PlayerCursor:ecs::component {

		voxtra::WorldRayCollision Hit;
		math::bounds look_range= math::bounds(0,4);
	};

	struct PlayerCursorCaster:ecs::System{
		void run(ecs::Ecs& world) {
			
			ecs::View<ecs::With<ecs::transform_comp>, ecs::With<PlayerCursor>,ecs::Owner>look_view(world);
			for (stn::TupleSet<ecs::transform_comp&,PlayerCursor&,ecs::obj> view: look_view) {
				
				PlayerCursor& cursor=view.get<PlayerCursor&>();
				if (world.get_resource<ui::MenuState>().menu_open()) {
					cursor.Hit = stn::None;
					continue;
				}
				math::Transform& transform= view.get<ecs::transform_comp&>().transform;
				math::ray look_ray=transform.forward_ray().dialate_from_start(cursor.look_range.max());

				cursor.Hit = collision::raycast(look_ray, collision::HitQuery(view.get<ecs::obj>()));
				if (cursor.Hit) {
					voxtra::RayWorldHit hit = cursor.Hit.unwrap();
					world.write_event(CursorHit{.hit=hit.owner() });
				}
			}
		}

	};

	struct OpenMenuOnClick :ecs::component {
		OpenMenuOnClick(ecs::ConstrainedHandle<ui::menu_component>&& hand) :handle(std::move(hand)) {

		}
		ecs::ConstrainedHandle<ui::menu_component> handle;
	};
	struct MenuClickSystem :ecs::System {
		MenuClickSystem(ecs::Ecs& world) :hits(world.make_reader<player::CursorHit>()) {

		}
		//todo add click_menu component
		void run(ecs::Ecs& world) {
			for (player::CursorHit hit : hits.read()) {
				if (world.get_resource<userinput::InputManager>().right_mouse().pressed) {
					if (hit.hit.has_component<OpenMenuOnClick>()) {
						world.write_command(ui::open_menu(hit.hit.get_component<OpenMenuOnClick>().handle.object()));
					}
				}
			}
		}
	private:
		ecs::EventReader<player::CursorHit> hits;
	};
	struct PlayerClickablePlugin :Core::Plugin {
		void build(Core::App& world) {
			world.emplace_system<MenuClickSystem>();
			world.emplace_system<PlayerCursorCaster>();
		}
	};
}