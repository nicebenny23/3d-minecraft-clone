#pragma once
#include "../renderer/uibox.h"
#include "../game/ecs/game_object.h"
#include "../game/collision.h"
#include "../world/managegrid.h"

#include "../world/voxeltraversal.h"
#include "../util/dynamicarray.h"
#include "../debugger/debug.h"
#include "playerinventory.h"

#include "../util/cached.h"
#include "../game/ecs/query.h"
#include "player_look.h"
#include "../game/transforms.h"
#include "../items/block_definitions.h"
#include "../renderer/ModelMesh.h"
#include "../game/time.h"
// Picks the BlockFace of the block that the point lies closest to
inline math::Direction3d closest_face(v3::Point3 pos, blocks::block& blk) {
	Vec3 offset_pos = (pos - blk.center())/blk.bounds().scale;
	math::Direction3d best = math::up_3d;
	for (auto d : math::Directions3d) {
		if (dot(d.vec(), offset_pos) > dot(best.vec(), offset_pos)) best = d;
	}
	return best;
}

// Returns two orthogonal axes on the block BlockFace plane
inline stn::pair<v3::Vec3, v3::Vec3> get_flat_frame(math::Direction3d dir) {
	switch (dir.axis()) {
	case math::AxisIndex3d::Right: return { v3::Vec3(0,0,1), v3::Vec3(0,1,0) };
	case math::AxisIndex3d::Up: return { v3::Vec3(1,0,0), v3::Vec3(0,0,1) };
	case math::AxisIndex3d::Front: return { v3::Vec3(1,0,0), v3::Vec3(0,1,0)};
	}
}
namespace player {
	struct PlayerBreaker {
		ecs::Constrained<block> current_mining;
		double amount_done;
		block& current_block() {
			return current_mining.get<block>();
		}
		inline size_t decal_value() {
			return math::clamp(amount_done * 7,0,6);
		}
	};
	struct playerbreak : ecs::component {
		ecs::obj break_decal;
		stn::Option< PlayerBreaker> breaker;
		
		void start() override {
			break_decal = ecs::spawn_emplaced<DecalRecipe>(world(),TexturePath("images\\block_break_2.png"));
			//	CtxName::ctx.Ren->Textures.LoadTexture("images\\menutex.png");
		}

		void spawn_decal(size_t phase,PlayerCursor& look) {
			if (phase == 0) return;

			static const char* tex[] = { "",
				"images\\block_break_1.png","images\\block_break_2.png",
				"images\\block_break_3.png","images\\block_break_4.png",
				"images\\block_break_5.png","images\\block_break_6.png" };
			std::string path = tex[phase];
			break_decal.get_component<renderer::Model>().texture= world().load_asset(renderer::TexturePath(path));
			math::Transform& transform= break_decal.get_component<core::LocalTransform>().transform;

			break_decal.get_component<renderer::Model>().enabled=true;
			auto hit = look.hit.unwrap().intersection();
			block& blk= breaker.unwrap().current_block();
			math::Direction3d fd = closest_face(hit, blk);
			auto f = breaker.unwrap().current_block().mesh[fd.index()];
			transform.position= f.center();
			transform.look_towards(fd.vec());

		}

		playerbreak() {
		};

		float curr_mining_power(stn::Option<ecs::Constrained<items::item_stack>> pickaxe) {
			if (!pickaxe) {
				return 1;
			}
			 items::item_stack& stack= pickaxe.unwrap().get<items::item_stack>();

			 return stack.types()
			.from_id(stack.contained_id())
			.tool_info()
			.member(&items::tool_traits::pickaxe_speed)
			.unwrap_or(1);
			

		}
		void engage_block(ecs::obj blk) {
			
			if (!breaker||breaker.unwrap().current_mining.object()!=blk) {
				breaker = PlayerBreaker{ .current_mining = blk,.amount_done = 0 };
			}
		}
		void disengage_block() {
			breaker = stn::None;
			break_decal.get_component<renderer::Model>().enabled=false;
		}
		bool engaged() {
			return breaker.is_some();
		}
		//returns current speed
		bool ensure_engage(player::PlayerCursor& cursor, stn::Option<ecs::Constrained<items::item_stack>> pick) {
			if (!cursor.hit) {
				return false;
			}
			voxtra::RayWorldHit& hit = cursor.hit.unwrap();
			if (!hit.owner().has_component<blocks::block>()) {
				return false;
			}

			if (!world().ensure_resource<userinput::InputManager>().left_mouse().held) {
				return false;
			}

			if (curr_mining_power(pick)<hit.owner().get_component<blocks::block>().type()->mining_traits().power_level ) {
				return false;
			}
			engage_block(hit.owner());

			return true;
		}

		void try_modify(player::PlayerCursor& cursor,stn::Option<ecs::Constrained<items::item_stack>> pick) {
			

			if (ensure_engage(cursor,pick)) {
				
				PlayerBreaker& player_break = breaker.unwrap();
				double power =1.0/ player_break.current_block().type()->mining_traits().time_to_mine;
				if (player_break.current_block().type()->mining_traits().pick_speedup) {
					power*= curr_mining_power(pick);
				}
				player_break.amount_done+=power*world().ensure_resource<timing::WorldClock>().dt;
				// Show progress decal
				
				if (player_break.current_block().bounds().scale == blockscale) {
					spawn_decal(player_break.decal_value(), cursor);
				}
				if (1<=player_break.amount_done) {
						on_break(player_break.current_mining, pick);
				}
			}
			else {
				disengage_block();
			}
		}

		void on_break(ecs::Constrained<block> broken, stn::Option<ecs::Constrained<items::item_stack>> pickaxe) {

			// Break when timer completes
			if (pickaxe.is_some_and(&ecs::Constrained<items::item_stack>::has_component<items::item_durability>)) {
				items::item_durability& duribility = pickaxe.unwrap().get_component<items::item_durability>();
				duribility.use();
			}
			block& broken_block = broken.get_component<block>();
			if (broken.has_component<items::loot_dropper>()) {
			broken.get_component<items::loot_dropper>().set(owner());
			}
			if (broken_block.is<blocks::LogBlock>()) {
				grid::set_block(world(), broken_block.pos, broken_block.registry->get_id<blocks::SeedBlock>());

			}
			else {
				grid::set_block(world(), broken_block.pos, broken_block.registry->get_id<AirBlock>());

			}
			disengage_block();
		}

	};

	struct PlayerUpdateSystem : ecs::System {
		void run(ecs::Ecs& ecs) override {
			for (auto [pb, look,inventory] : ecs::View< playerbreak, player::PlayerCursor, player::inventory>(ecs)) {
				pb.try_modify(look,inventory.selected_object());
			};
		}
	};
	struct PlayerModificationPlugin {
		void operator()(core::App& world) {
			world.insert_plugin(player::PlayerClickablePlugin());
			world.emplace_system<PlayerUpdateSystem>();

		}

	};
}