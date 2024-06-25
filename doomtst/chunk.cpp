#include "chunk.h"

inline int chunk::indfrompos(int x, int y, int z)
{
return	256 * x + 16 * y + z;
}
void giveblocktraits(blockname::block* nullblock) {
	switch (nullblock->id)
	{
	case minecraftair:
		break;
	case minecraftgrass:
		nullblock->texture = 1;

		nullblock->createaabb();
		break;
	case minecraftdirt:
		nullblock->texture = 0;

		nullblock->createaabb();
		break;
	}
};
chunk::chunk* chunk::load(int xind,int zind)
{
	chunk* retchunk = new chunk();
	retchunk->xchunk = xind;
	retchunk->zchunk = zind;
	retchunk->blockstruct = new block[16 * 16 * 16];
	for (int i = 0; i < 16 * 16 * 16; i++)
	{
		retchunk->blockstruct[i] = blockname::block(iv3::zerov, 0);
		
	
	}
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0;y < 16;y++) {
			for (int z = 0; z < 16; z++)
			{
				int ind = 256 * x + 16 * y + z;
				retchunk->blockstruct[ind].pos = iv3::Ivector3(x + 16 * xind, y, z + 16 * zind);
				
				//select block mechanism
				if (y==10)
				{
					retchunk->blockstruct[ind].id = minecraftgrass;
				}
				if (y <10)
				{
					retchunk->blockstruct[ind].id = minecraftdirt;
				}
				if (10<y)
				
				{
					retchunk->blockstruct[ind].id = minecraftair;
				}
				giveblocktraits(&(retchunk->blockstruct[ind]));
			}
		}
	}
	return retchunk;
}

void chunk::chunk::renderchunk()
{
	for (int x= 0; x < 16; x++)
	{
		for (int y = 0;y < 16;y++) {
			for (int z = 0; z < 16; z++)
			{
				int ind = 256 * x + 16 * y + z;

				blockstruct[ind].render();

			}


		}
	}
}

chunk::chunk::chunk()
{
	xchunk = 0;
	zchunk = 0;
	blockstruct = 0;
}

void chunk::chunk::destroy()
{
	for (int i = 0;i < 16 * 16 * 16;i++) {
		gameobject::objectfromguid[blockstruct[i].guid] = nullptr;
	
	gameobject::immidiatedestroy(&blockstruct[i]);

	}
	delete[] blockstruct;
}
