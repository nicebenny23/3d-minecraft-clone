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
#include "chest.h"
#include "furnaceblock.h"
#include "ironblock.h"
#include "altar.h"
#include "plank.h"
#include "sand.h"
//provides utility functions for blocks
using namespace blockname;
#pragma once  
namespace blkinitname {

	inline void blockinit(block* blk) {
		///blk->bstate.broken = false;
	switch (blk->id)
		{
		case minecraftair:
			setdefault(blk);
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
		case minecraftsand:
			sandinit(blk);
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
		case minecraftchest:
			chestinit(blk);
			break;
		case minecraftfurnace:
			furnaceinit(blk);
			break;
		case minecraftironore:
			ironinit(blk);
			break;
		case minecraftaltar:
			altarinit(blk);
			break;
		case minecraftplank:
			plankinit(blk);
			break;
		}

	}
	inline void genblock(block* blk, int blkid, Coord location, byte attachface, byte direction)  {

		blk->create(location, blkid,Dir::Dir3d(attachface),Dir::Dir2d(direction));


		blkinitname::blockinit(blk);


	}
	
}