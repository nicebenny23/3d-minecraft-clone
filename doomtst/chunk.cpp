#include "chunk.h"

inline int chunk::indfrompos(int x, int y, int z)
{
return	256 * x + 16 * y + z;
}

chunk::chunk* chunk::load(int xind,int zind)
{
	chunk* retchunk = new chunk();
	retchunk->xchunk = xind;
	retchunk->zchunk = zind;
	retchunk->blockstruct = (new rblock[16*16*16]);
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0;y < 16;y++) {
			for (int z = 0; z < 16; z++)
			{
				int ind = 256 * x + 16 * y + z;
				retchunk->blockstruct[ind].pos = Vector3(x + 16 * xind, y, z + 16 * zind);
				if (0<y&&(y<14||(x==floor(x/2)*2&&z== floor((z +x+y)/ 3))))
				{
					retchunk->blockstruct[ind].texture = 0;
					retchunk->blockstruct[ind].id = minecraftgrass;
					retchunk->blockstruct[ind].createaabb();
				}
				else
				{
					retchunk->blockstruct[ind].id = minecraftair;
				}
				
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
	xchunk = -1000;
	zchunk = -1000;
	blockstruct = 0;
}

void chunk::chunk::destroy()
{
	delete[] blockstruct;
}
