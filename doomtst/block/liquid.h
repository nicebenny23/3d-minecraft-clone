#include "../game/gameobject.h"
#include "../block/block.h"
#include "../game/objecthelper.h"
#include "../world/grid.h"
#include "../world/managegrid.h"
#include "../util/time.h"
#ifndef  liquid_HPP
struct liquidprop :gameobject::component {
	int maxliq;
	int liqval;
	liquidprop(int value) {

		maxliq =value;
		liqval = value;
	}
	float diffusetime;
	void start() {
		diffusetime = 0;
		 
		
	}
	void updateinface(int face) {
		Coord newpos = dirfromint(face) + objutil::toblk(owner).pos;
		blockname::block* blk = grid::getobjatgrid(newpos, true);
		if (blk==nullptr)
		{
			return;
		}
		if (blk->id != minecraftair)
		{
			if (blk->id == minecrafttorch)
			{

				gridutil::setblock(newpos, minecraftair);
			}
			return;
		}
		gridutil::setblock(blk->pos, objutil::toblk(owner).id);
		if (!blk->hascomponent<liquidprop>())
		{
			Assert("block must be inititated with liquid component");
		}
		if (face==3)
		{
			blk->getcomponent<liquidprop>().liqval = liqval;
		}
		else {
			blk->getcomponent<liquidprop>().liqval = liqval - 1;
		}
		
		blk->mesh.attachdir = 3;
		
	}
	void update() {
		diffusetime += timename::dt;
		if (1<liqval&&.4<diffusetime)
		{
			updateinface(0);
			updateinface(1);
			updateinface(3);
			updateinface(4);

			updateinface(5);
		}
		
	}



};
#define liquid_HPP
#endif // ! water_Hpp
