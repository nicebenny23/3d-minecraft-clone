#pragma once
#include "../game/ecs/game_object.h"
#include "../game/collision.h"
#include "../world/managegrid.h"

#include "../world/voxeltraversal.h"
#include "playerinventory.h"
#include "player_look.h"

namespace player {
	struct player_place : ecs::component {
	
	};
	struct PlayerPlaceSystem : ecs::System {
		void run(ecs::Ecs& ecs) override {
			if (!ecs.get_resource<userinput::InputManager>().right_mouse().pressed) {
				return;
			}
			for (auto [pb, look, inventory] : ecs::View< player_place, player::PlayerCursor, player::inventory>(ecs)) {
			

				if (!inventory.selected() || !look.hit) {
					continue;
				}
				items::item_stack& stack = inventory.selected().unwrap();
				if (!stack.types().from_id(stack.contained_id()).traits(ecs).blk_id) {
					continue;
				}
				if (!stack.can_remove(1)) {
					continue;
				}
				block_id spawn_id = stack.traits().blk_id.unwrap();

				grid::Grid& grid = ecs.get_resource<grid::Grid>();
				voxtra::RayWorldHit hit = look.hit.unwrap();
				if (!hit.owner().has_component<block>()) {
					continue;
				}
				
				if (hit.owner().has_component<player::OpenMenuOnClick>()) {
					continue;
				}
				block& block_hit_at = hit.owner().get_component<block>();

				double eps = .0001;
				stn::Option<block&> mabye_block_at = look.backpedal.get<block>();
				if (!mabye_block_at) {
					continue;
				}
				BlockRegistry& blocks = ecs.get_resource<BlockRegistry>();
				block& block_at = mabye_block_at.unwrap();
				math::Direction3d attach_direction = math::greatest_aligned_direction( block_hit_at.center()- block_at.center());
				v3::Scale3 mesh_scale = block_at.registry->get_block(spawn_id).mesh_traits_for().size;
				geo::Box new_block_box = geo::Box(block_at.pos+unitv/2, mesh_scale);
				new_block_box.center -= attach_direction.vec() * mesh_scale.shrunk(1)/2;

				double block_push_margin = .2f;
				new_block_box.scale.shink(block_push_margin);
				if (collision::boxcast(new_block_box,collision::HitQuery(ecs))) {
					continue;
				}
				if (blocks.name_for(spawn_id)=="seed") {
					stn::Option<ecs::Constrained<block>&> below = grid.get_object(block_at.pos - Coord(0, 1, 0));
					if (!(below&&below.unwrap().has_component<Seedability>())) {
						continue;
					}
				}
   					grid::set_block(ecs, block_at.pos, spawn_id,attach_direction);
					stack.remove(1);
			}
		}
	};
	struct PlayerPlacePlugin {
		void operator()(core::App& world) {
			world.insert_plugin(player::PlayerClickablePlugin());
			world.emplace_system<PlayerPlaceSystem>();

		}

	};

};
