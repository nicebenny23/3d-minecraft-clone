#pragma once
#include "../game/ecs/game_object.h"
#include "../game/collision.h"
#include "../world/managegrid.h"
#include "../game/objecthelper.h"
#include "../world/voxeltraversal.h"
#include "playerinventory.h"
#include "player_look.h"

namespace player {
	struct player_place : ecs::component {
	
	};
	struct PlayerPlaceSystem : ecs::System {
		void run(ecs::Ecs& ecs) override {
			for (auto [pb, look, inventory] : ecs::View<ecs::With<player_place>, ecs::With<player::PlayerCursor>, ecs::With<player::inventory>>(ecs)) {
				if (!inventory.selected() || !look.Hit) {
					continue;
				}
				items::item_stack& stack = inventory.selected().unwrap();
				if (!stack.types().from_id(stack.contained_id()).blk_id) {
					continue;
				}
				if (!stack.can_remove(1)) {
					continue;
				}
				block_id spawn_id = stack.types().from_id(stack.contained_id()).blk_id.unwrap();

				grid::Grid& grid = ecs.get_resource<grid::Grid>();
				voxtra::RayWorldHit hit = look.Hit.unwrap();
				if (!hit.owner().has_component<block>()) {
					continue;
				}
				block& block_hit_at = hit.owner().get_component<block>();

				double eps = .0001;
				v3::Point3 backpedal_point = hit.ray().point_at(hit.dist()- eps);
  				stn::Option<block&> mabye_block_at = grid.get_block(grid.get_voxel(backpedal_point));
				if (!mabye_block_at) {
					continue;
				}
				block& block_at = mabye_block_at.unwrap();
				math::Direction3d attach_direction = math::greatest_aligned_direction(block_at.center() - block_hit_at.center());
				v3::Scale3 mesh_scale = block_at.registry().block_for(spawn_id).block_traits.mesh.size;
				Box new_block_box = Box(block_at.center(), mesh_scale);
				new_block_box.center -= attach_direction.vec() * mesh_scale.shrunk(.5f);
				new_block_box.scale.shrunk(.01f);
				if (ecs.get_resource<userinput::InputManager>().right_mouse().pressed) {
   					grid::set_block(ecs, block_at.pos, spawn_id);
					stack.remove(1);
				};
			}
		}
	};
	struct PlayerPlacePlugin :Core::Plugin {
		void build(Core::App& world) {
			world.emplace_system<player::PlayerCursorCaster>();
			world.emplace_system<PlayerPlaceSystem>();

		}

	};

};
