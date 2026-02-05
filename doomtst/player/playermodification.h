#pragma once
#include "../renderer/uibox.h"
#include "../game/ecs/game_object.h"
#include "../game/collision.h"
#include "../world/managegrid.h"
#include "../game/objecthelper.h"
#include "../world/voxeltraversal.h"
#include "../util/dynamicarray.h"
#include "../debugger/debug.h"
#include "playerinventory.h"
#include "../renderer/decal.h"
#include "../util/cached.h"
#include "../game/ecs/query.h"

// Picks the face of the block that the point lies closest to
inline math::Direction3d closest_face(v3::Point3 pos, block* blk) {
	Vec3 offset_pos = pos - blk->center();
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

void initbreakparticle(ecs::obj newent);

struct playerbreak : ecs::component {
	voxtra::WorldRayCollision closest;
	stn::change <stn::Option<ecs::obj>> pickaxe;

	decal break_decal;
	float break_start_time = 0.f;
	float timeuntilbreak = 0.f;
	stn::change<block*> currmining;

	void start() override {
		currmining();
		currmining.clear(nullptr);
	//	CtxName::ctx.Ren->Textures.LoadTexture("images\\menutex.png", "MenuTexture");
	}

	void spawn_decal(size_t phase) {
		if (phase == 0) return;

		static const char* tex[] = { "",
			"images\\block_break_1.png","images\\block_break_2.png",
			"images\\block_break_3.png","images\\block_break_4.png",
			"images\\block_break_5.png","images\\block_break_6.png" };
		std::string name = tex[phase];
		std::string handle = "block_break_" + std::to_string(phase);
		break_decal.set_handle(name.c_str(), handle.c_str());
		break_decal.handle.enable();
		auto hit = closest.unwrap().Hit.intersectionpoint;
		math::Direction3d fd = closest_face(hit, currmining());
		auto& f = currmining()->mesh[fd.index()];
		break_decal.center = f.center();
		break_decal.normal = fd.vec();
		auto [b, t] = get_flat_frame(fd);
		break_decal.bi_tangent = b * 0.5f;
		break_decal.tangent = t * 0.5f;
		break_decal.create_mesh();
		break_decal.render();
	}

	playerbreak() {
	};
	float block_power(block* blk) {
		return blk->mininglevel;

	}
	float curr_mining_power() {
		if (!pickaxe()) {
			return 1;
		}
		return 1;

	}
	void engage_block(block* blk) {
		if (!engaged()) {
			if (break_decal.handle) {
				// break_decal.handle.disable();
			}
			pickaxe.clear(owner().get_component<inventory>().selected().map(&component::owner));
			currmining.clear(blk);
			break_start_time = block_power(blk);
			timeuntilbreak = break_start_time;

		}
		else {
			pickaxe.set(owner().get_component<inventory>().selected().map(&component::owner));
			currmining.set(blk);
		}
	}
	void disengage_block() {
		currmining.clear(nullptr);
		pickaxe.clear(stn::None);
		if (break_decal.handle) {
			break_decal.handle.disable();
		}
		timeuntilbreak = -1;

	}
	bool engaged() {
		return currmining() != nullptr;
	}
	//returns current speed
	bool ensure_engage() {
		ray r= ray::from_offset(owner().get_component<ecs::transform_comp>().transform.position,owner().get_component<ecs::transform_comp>().transform.normal_dir() * 7.f);
		closest = collision::raycastall(r, collision::HitQuery(owner()), voxtra::countsolid);
		if (!closest) {
			return false;
		}
		auto hit = closest.unwrap();
		if (!hit.owner().has_component<blocks::block>()) {
			return false;
		}
		if (!inrange(hit.dist(), interactminrange, interactmaxrange)) {
			return false;
		}
		if (!CtxName::ctx.Inp->left_mouse().held) {
			return false;
		}
		engage_block(hit.owner().get_component_ptr<block>());
		if (engaged()) {
			if (currmining.changed() ) {
				return false;
			}
		}

		return true;
	}

	// Called each frame while holding the mouse;
	void try_modify() {
		if (ensure_engage()) {

			timeuntilbreak -= curr_mining_power() * CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().dt;
			// Show progress decal
			float prog = (break_start_time - timeuntilbreak) / break_start_time;
			size_t phase = clamp(size_t(prog * 7.f), 0, 6);
			if (currmining()->scale() == blockscale) {
				spawn_decal(phase);
			}
			if (timeuntilbreak <= 0.f) {
				on_break(currmining());

			}
		}
		else {
			disengage_block();
		}
	}

	void on_break(block* broken) {

		// Break when timer completes
		if (timeuntilbreak <= 0.f) {
			if (pickaxe()) {
				pickaxe().unwrap().get_component_opt<items::item_durability>();
			}
			if (!currmining()->minedfastwithpick || currmining()->mininglevel <= curr_mining_power()) {
				make_drop(broken->owner());
			}
			grid::set_block(world(),broken->pos, minecraftair);
			disengage_block();

		}
	}
	void update() {
		try_modify();
	}
	void make_drop(ecs::obj Hit);

};

struct PlayerUpdateSystem : ecs::System {
	void run(ecs::Ecs& ecs) override {
		for (auto [pb] : ecs::View<playerbreak>(ecs)) {
			pb.try_modify();
		};
	}
};
