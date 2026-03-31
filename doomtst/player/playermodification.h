#pragma once
#include "../renderer/uibox.h"
#include "../game/ecs/game_object.h"
#include "../game/collision.h"
#include "../world/managegrid.h"

#include "../world/voxeltraversal.h"
#include "../util/dynamicarray.h"
#include "../debugger/debug.h"
#include "playerinventory.h"
#include "../renderer/decal.h"
#include "../util/cached.h"
#include "../game/ecs/query.h"
#include "player_look.h"
// Picks the face of the block that the point lies closest to
inline math::Direction3d closest_face(v3::Point3 pos, block& blk) {
	Vec3 offset_pos = pos - blk.center();
	math::Direction3d best = math::up_3d;
	for (auto d : math::Directions3d) {
		if (dot(d.vec(), offset_pos) > dot(best.vec(), offset_pos)) best = d;
	}
	return best;
}

// Returns two orthogonal axes on the block face plane
inline stn::pair<v3::Vec3, v3::Vec3> get_flat_frame(math::Direction3d dir) {
	switch (dir.axis()) {
	case math::AxisIndex3d::Right: return { v3::Vec3(0,0,1), v3::Vec3(0,1,0) };
	case math::AxisIndex3d::Up: return { v3::Vec3(1,0,0), v3::Vec3(0,0,1) };
	case math::AxisIndex3d::Front: return { v3::Vec3(1,0,0), v3::Vec3(0,1,0)};
	}
}
namespace player {
	struct playerbreak : ecs::component {
		ecs::obj break_decal;
		float break_start_time = 0.f;
		float timeuntilbreak = 0.f;
		stn::Option<ecs::obj> currmining;

		void start() override {
			break_decal = ecs::spawn_emplaced<decals::DecalSpawner>(world(), v3::Point3(0, 0, 0));
			//	CtxName::ctx.Ren->Textures.LoadTexture("images\\menutex.png", "MenuTexture");
		}

		void spawn_decal(size_t phase,PlayerCursor& look) {
			if (phase == 0) return;

			static const char* tex[] = { "",
				"images\\block_break_1.png","images\\block_break_2.png",
				"images\\block_break_3.png","images\\block_break_4.png",
				"images\\block_break_5.png","images\\block_break_6.png" };
			std::string path = tex[phase];
			std::string handle = "block_break_" + std::to_string(phase);
			world().emplace_command<decals::DecalReimageCommand>(break_decal, renderer::TexturePath(path, handle));
			decals::decal_component& decal = break_decal.get_component<decals::decal_component>();
			decal.enable();
			auto hit = look.Hit.unwrap().intersection();
			block& blk= currmining.unwrap().get_component<block>();
			math::Direction3d fd = closest_face(hit, blk);
			auto f = currmining.unwrap().get_component<block>().mesh[fd.index()];
			decal.center = f.center();
			decal.normal = fd.vec();
			auto [b, t] = get_flat_frame(fd);
			decal.bi_tangent = (b * 0.5f);
			decal.tangent = (t * 0.5f);


		}

		playerbreak() {
		};

		float curr_mining_power(stn::Option<ecs::obj> pickaxe) {
			if (!pickaxe) {
				return 1;
			}
			 items::item_stack& stack= pickaxe.unwrap().get_component<items::item_stack>();

			 return stack.types()
			.from_id(stack.contained_id())
			.tool
			.member(&items::tool_traits::pickaxe_speed)
			.unwrap_or(1);
			

		}
		void engage_block(ecs::obj blk) {
			
			if (currmining!=blk) {
				currmining = blk;
				break_start_time = blk.get_component<block>().type()->mining_traits().time_to_mine;
				timeuntilbreak = break_start_time;
			}
		}
		void disengage_block() {
			currmining.clear();
			if (engaged()) {
				int l = 4;
			}
			break_decal.get_component<decals::decal_component>().disable();
			timeuntilbreak = -1;

		}
		bool engaged() {
			return currmining.is_some();
		}
		//returns current speed
		bool ensure_engage(player::PlayerCursor& cursor) {
			if (!cursor.Hit) {
				return false;
			}
			voxtra::RayWorldHit& hit = cursor.Hit.unwrap();
			if (!hit.owner().has_component<blocks::block>()) {
				return false;
			}

			if (!world().ensure_resource<userinput::InputManager>().left_mouse().held) {
				return false;
			}
			engage_block(hit.owner());
			return currmining.is_some();
		}

		void try_modify(player::PlayerCursor& cursor,stn::Option<ecs::obj> pick) {
			
			if (ensure_engage(cursor)) {
				timeuntilbreak -= curr_mining_power(pick) * world().ensure_resource<timename::TimeManager>().dt;
				// Show progress decal
				float prog = (break_start_time - timeuntilbreak) / break_start_time;
				size_t phase = clamp(size_t(prog * 7.f), 0, 6);
				if (currmining&&currmining.unwrap().get_component<block>().bounds().scale == blockscale) {
					spawn_decal(phase,cursor);
				}
				if (timeuntilbreak <= 0.f) {
					if (currmining) {
							on_break(currmining.unwrap().get_component<block>(), pick);
					}
				}
			}
			else {
				disengage_block();
			}
		}

		void on_break(block& broken, stn::Option<ecs::obj> pickaxe) {

			// Break when timer completes
			if (pickaxe.is_some_and(&ecs::obj::has_component<items::item_durability>)) {
				items::item_durability& duribility = pickaxe.unwrap().get_component<items::item_durability>();
				duribility.use();
			}
			
			if (broken.type()->mining_traits().time_to_mine <= curr_mining_power(pickaxe)) {
				make_drop(broken.owner());
			}
			grid::set_block(world(), broken.pos, broken.registry().get_id<AirBlock>());
			disengage_block();
		}
		void make_drop(ecs::obj Hit);

	};

	struct PlayerUpdateSystem : ecs::System {
		void run(ecs::Ecs& ecs) override {
			for (auto [pb, look,inventory] : ecs::View< playerbreak, player::PlayerCursor, player::inventory>(ecs)) {
				pb.try_modify(look,inventory.selected_object());
			};
		}
	};
	struct PlayerModificationPlugin :Core::Plugin {
		void build(Core::App& world) {
			world.insert_plugin<player::PlayerClickablePlugin>();
			world.emplace_system<PlayerUpdateSystem>();

		}

	};
}