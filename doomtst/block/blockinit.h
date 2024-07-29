#include "block.h"	
#include "air.h"
#include "treestone.h"
#include "moss.h"
#include "stone.h"
#include "glass.h"
#include "water.h"
#include "crystalore.h"
#include "torch.h"
#include "wood.h"
#include "craftingtable.h"
#include "ropeblock.h"
#include "crystaltorch.h"
#include "lava.h"
#include "obisidian.h"
//provides utility functions for blocks
using namespace blockname;
#ifndef blockinit_H
#define blockinit_H
namespace blkinitname {

	inline void blockinit(block* blk) {

		switch (blk->id)
		{
		case minecraftair:
			airinit(blk);
			break;
		case minecrafttreestone:
			treestoneinit(blk);
			break;
		case minecraftmoss:
			mossinit(blk);
			break;
		case minecraftstone:
			stoneinit(blk);
			break;
		case minecraftglass:
			glassinit(blk);
			break;
		case minecraftwater:
			waterinit(blk);
			break;
		case minecraftcrystal:
			crystalinit(blk);
			break;
		case minecrafttorch:
			torchinit(blk);
			break;
		case minecraftwood:
			woodinit(blk);
			break;
		case minecraftcraftingtable:
			tableinit(blk);
			break;
		case minecraftcrystaltorch:
			crystaltorchinit(blk);
			break;
		case minecraftrope:
			ropeinit(blk);
			break;
		case minecraftlava:
			lavainit(blk);
			break;
		case minecraftobsidian:
			obsidianinit(blk);
			break;
		}

		blk->state = gameobject::active;
	}
	inline void genblock(block* blk, int blkid, Coord location, byte attachface, byte direction) {

		*blk = blockname::block(location, blkid);

		initblockmesh(blk, zerov, unitscale);

		blkinitname::blockinit(blk);


	}
	inline void setair(block* blk) {
		blk->state = gameobject::beingsoftdestroyed;
		switch (blk->id)
		{
		case minecraftair:

			break;
		case minecraftmoss:
			mossdelete(blk);
			break;
		case minecrafttreestone:
			treestonedelete(blk);
			break;
		case minecraftstone:


			stonedelete(blk);
			break;

		case minecraftglass:


			glassdelete(blk);
			break;
		case minecraftcrystal:


			crystaldelete(blk);
			break;
		case minecrafttorch:


			torchdelete(blk);
			break;
		case minecraftwood:

			wooddelete(blk);
			break;
		case minecraftwater:


			waterdelete(blk);
			break;
		case minecraftcraftingtable:
			tabledelete(blk);
			break;
		case minecraftcrystaltorch:
			crystaltorchdelete(blk);
			break;
		case minecraftrope:
			ropedelete(blk);
			break;
		case minecraftlava:
			lavadelete(blk);
			break;
		case minecraftobsidian:
			obsidiandelete(blk);
			break;
		default:
			Assert("need valid id in set blk");
		}
		airinit(blk);

	}
	inline void deleteandset(block* blk, int id)
	{
		setair(blk);
		blk->id = id;
		blockinit(blk);
	
	}
}

#endif // !blockinit_H
