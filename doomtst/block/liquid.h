#include "../game/gameobject.h"
#include "../block/block.h"
#include "../game/objecthelper.h"
#include "../world/grid.h"
#include "../world/managegrid.h"
#include "../util/time.h"
#include "../game/rigidbody.h"
#ifndef  liquid_HPP
#define liquid_HPP
extern float liquidtick;

void updateltick();
struct liquidprop :gameobject::component {
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
			collidedwith->getcomponent<rigidbody>().velocity *= 1 - timename::dt * 5;
		}
	}
	void updateinface(int face) {



		Coord newpos = dirfromint(face) + objutil::toblk(owner).pos;

		blockname::block* blk = grid::getobjatgrid(newpos, true);
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


					if (blk->getcomponent<liquidprop>().liqval < liqval) {

						blk->getcomponent<liquidprop>().liqval += 1;
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

		if (liqval <= 1 && face != 3)
		{
			return;
		}
		if (liqval < 1)
		{
			return;
		}
		gridutil::setblock(blk->pos, objutil::toblk(owner).id);

		if (!blk->hascomponent<liquidprop>())
		{
			Assert("block must be inititated with liquid component");
		}
	
				blk->getcomponent<liquidprop>().liqval = 1;
				liqval -= 1;
			
	
		blk->mesh.attachdir = 3;

	}

	void update() {
		


			diffusetime +=tick::tickdt;
			if (.4 < diffusetime)
			{
				diffusetime = 0;

				updateinface(3);
				updateinface(0);
				updateinface(1);
				updateinface(4);
				updateinface(5);
			}
			if (liqval <= 0)
			{
				gridutil::setblock(objutil::toblk(owner).pos, minecraftair);
			}
		
	}



};
#define liquid_HPP
#endif // ! water_Hpp
