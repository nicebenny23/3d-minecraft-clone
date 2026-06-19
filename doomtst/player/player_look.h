#include "../game/ecs/game_object.h"
#include "../game/collision.h"
#include "../player/player.h"
#include "../math/interval.h"
#include "../game/casts.h"
#include <utility>
#include "../items/menu.h"
#include "../renderer/renderer/wireframe_box.h"
#include "../game/aabb.h"
#include "../game/health.h"
#pragma once
namespace player {
	struct CursorHit {
		ecs::obj hit;
	};
	struct PlayerCursor:ecs::component {
		ecs::Constrained<WireFrame, core::LocalTransform> frame;
		PlayerCursor(ecs::Constrained<WireFrame,core::LocalTransform> wireframe) :frame(wireframe) {

		}
		bool hit_entity() const {
			return hit && hit.unwrap().collider.has_component<Health::EntityHealth>();
		}
		bool hit_block() const {
			return hit && hit.unwrap().collider.has_component<block>();
		}
		
		ecs::WeakConstrained<block> backpedal;
		voxtra::RayWorldCollision hit;
		math::bounds look_range= math::bounds(0,3);
	};

	struct PlayerCursorCaster:ecs::System{
		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>();
			ecs::View< core::LocalTransform,PlayerCursor,ecs::Owner>look_view(world);
			for (stn::TupleSet<core::LocalTransform&,PlayerCursor&,ecs::obj> view: look_view) {
				
				PlayerCursor& cursor=view.get<PlayerCursor&>();
				if (world.get_resource<ui::MenuState>().menu_open()) {
					cursor.hit = stn::None;
					continue;
				}
				math::Transform& transform= view.get<core::LocalTransform&>().transform;
				geo::ray look_ray=transform.forward_ray().dialate_from_start(cursor.look_range.max());
				
				cursor.hit = collision::raycast(look_ray, collision::HitQuery(view.get<ecs::obj>()));
				if (cursor.hit) {
					voxtra::RayWorldHit hit = cursor.hit.unwrap();
					
					world.write_event(CursorHit{.hit=hit.owner() });
					if (hit.owner().has_component<block>()) {

						double light = .5f;
						double eps = .0001;
						v3::Point3 backpedal_point = hit.ray().point_at(hit.dist() - eps);
						cursor.backpedal.set(grid.get_object(grid.get_voxel(backpedal_point)).copied());
						if (cursor.backpedal) {
							block& blk = cursor.backpedal.get<block>().unwrap();
							light += blk.light_passing_through / 16.0;

						}
						cursor.frame.get<WireFrame>().color = colors::Color::monotone(light);
						cursor.frame.get<WireFrame>().enabled = true;
						cursor.frame.get<core::LocalTransform>().transform.from_box_unrotated(aabb::global_box(hit.collider));
					}
				}
				else {
					view.get<PlayerCursor&>().frame.get<WireFrame>().enabled = false;
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
	struct PlayerClickablePlugin {
		void operator()(Core::App& world) {
			world.insert_plugin(renderer::wireframe_plugin);
			world.emplace_system<MenuClickSystem>();
			world.emplace_system<PlayerCursorCaster>();
		}
	};
}