#include "../game/gameobject.h"
#include "../block/block.h"
#include "../game/objecthelper.h"
#include "../world/grid.h"
#include "../world/managegrid.h"
#include "../util/time.h"
#include "../debugger/debug.h"
#include "../game/rigidbody.h"
#pragma once 
extern float liquidtick;

void updateltick();
struct liquidprop : gameobject::component {
	int maxliq;
	int liqval;

	liquidprop(int value) {
		int maxliquid = 0;
		maxliq = value;
	
		liqval = value;
	}
	float diffusetime;
	void start() {
		diffusetime = 0;
		utype = gameobject::updatetick;

	}
	void oncollision(gameobject::obj* collidedwith) {
		if (collidedwith->hascomponent<rigidbody>())
		{
			collidedwith->getcomponent<rigidbody>().inliquid = true;
			collidedwith->getcomponent<rigidbody>().velocity *= 1 - CtxName::ctx.Time->dt * 5;
		}
	}
	void updateinface(Dir::Dir3d face) {



		Coord newpos =face.ToVec() + objutil::toblk(owner).pos;

		blockname::block* blk = CtxName::ctx.Grid->getBlock(newpos);
		if (blk == nullptr)
		{
			return;
		}
		if (blk->id != minecraftair)
		{
			if (blk->id == objutil::toblk(owner).id)
			{
				if (liqval > 1)
				{


					if (blk->owner.getcomponent<liquidprop>().liqval < liqval) {

						blk->owner.getcomponent<liquidprop>().liqval += 1;
						liqval -= 1;
					}
				}
			}



			if (blk->id == minecrafttorch)
			{

				gridutil::setblock(newpos, minecraftair);
			}


			return;

		}

		//instert for what dor==3 was
		if (liqval <= 1)
		{
			return;
		}
		if (liqval < 1)
		{
			return;
		}
		gridutil::setblock(blk->pos, objutil::toblk(owner).id);

		if (!blk->owner.hascomponent<liquidprop>())
		{
			Assert("block must be inititated with liquid component");
		}
	
				blk->owner.getcomponent<liquidprop>().liqval = 1;
				liqval -= 1;
			
	
		blk->mesh.attachdir = Dir::up3d;

	}

	void update() {
		


			diffusetime +=tick::tickdt;
			if (.4 < diffusetime)
			{
				diffusetime = 0;

				for (auto dir: Dir::Directions3d)
				{
					if (dir != Dir::up3d) {

						updateinface(dir);
					}
				}
			}
			if (liqval <= 0)
			{
				gridutil::setblock(objutil::toblk(owner).pos, minecraftair);
			}
		
	}



};
#define liquid_HPP
 // ! water_Hpp
