#include "block.h"	
#include "air.h"
#include "dirt.h"
#include "grass.h"
#include "stone.h"
#include "glass.h"
#include "water.h"
#include "crystalore.h"
#include "torch.h"
#include "wood.h"
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
		case minecraftdirt:
			dirtinit(blk);
			break;
		case minecraftgrass:
			grassinit(blk);
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
		}
     } 
	inline void genblock(block* blk,int blkid, Coord location, byte attachface, byte direction) {
	
		 *blk = blockname::block(location, blkid);
	
		initblockmesh(blk, zerov, unitscale);
		
		blkinitname::blockinit(blk);
		
	
	}
	inline void setair(block* blk) {
		switch (blk->id)
		{
		case minecraftair:
			
			break;
		case minecraftgrass:
			grassdelete(blk);
			break;
		case minecraftdirt:
			dirtdelete(blk);
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
