#include "../game/ecs/game_object.h"
#include "../block/block.h"
#include "../world/grid.h"
#include "../world/managegrid.h"
#include "../game/time.h"
#include "../debugger/debug.h"

#include "../game/rigidbody.h"
#pragma once 
extern float liquidtick;

void updateltick();
struct liquidprop : ecs::component {
	int maxliq;
	int liqval;

	liquidprop(int value) {
		int maxliquid = 0;
		maxliq = value;

		liqval = value;
	}
	float diffusetime;
	void start() {
		if (!owner().has_component<block>()) {
			throw std::logic_error("all liquids must be blocks");
		}
		diffusetime = 0;
		//utype = ecs::updatetick;

	}
	void oncollision(ecs::obj* collidedwith) {
		if (collidedwith->has_component<rigidbody>()) {
			collidedwith->get_component<rigidbody>().inliquid = true;
			collidedwith->get_component<rigidbody>().velocity *= 1 - CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().dt * 5;
		}
	}
	void updateinface(math::Direction3d face) {

		if (liqval <= 1) {
			return;
		}
		Coord newpos = face.coord() + owner().get_component<block>().pos;
		blocks::block* blk = CtxName::ctx.Grid->getBlock(newpos);
		if (blk == nullptr) {
			return;
		}

		//instert for what dor==3 was

		if (blk->id != minecraftair) {
			//if (blk->block_id == owner()) {
				if (liqval > 1) {


					if (blk->owner().get_component<liquidprop>().liqval < liqval) {

						blk->owner().get_component<liquidprop>().liqval += 1;
						liqval -= 1;
					}
			//}
			}



		

			return;


		}

		Coord pos = blk->pos;
		grid::set_block(world(), pos, owner().get_component<blocks::block>().id);
		blk = (CtxName::ctx.GridRef().getBlock(pos));

		if (!blk->owner().has_component<liquidprop>()) {
			Assert("block must be inititated with liquid component");
		}

		blk->owner().get_component<liquidprop>().liqval = 1;
		liqval -= 1;


		blk->mesh.attached_direction = math::up_3d;

	}

	void update() {
	//	diffusetime += tick::tickdt;
		if (.4 < diffusetime) {
			diffusetime = 0;

			for (auto dir : math::Directions3d) {
				if (dir != math::up_3d) {

					updateinface(dir);
				}
			}
		}
		if (liqval <= 0) {
			grid::set_block(world(),owner().get_component<blocks::block>().pos, minecraftair);
		}

	}



};