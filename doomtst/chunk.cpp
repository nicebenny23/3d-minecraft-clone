#include "chunk.h"
#include "noise.h"
inline int modabs1(int x, int m) {

	if (x < 0)
	{
		if (static_cast<double>(x) / m == int(x / m))
		{
			//so (-16,-16) is now a part of the correct chunk this has to bedon due to the inverse taken
			return 0;
		}
		else {
			return m - ((-x) % m);
		}
	}
	else
	{
		return x % m;
	}
}
inline int chunk::indfromlocalpos(int x, int y, int z)
{
return	256 * x + 16 * y + z;
}
int chunk::indexfrompos(int x, int y, int z)
{
	x = modabs1(x, 16);
	y = modabs1(y, 16);
	z = modabs1(z, 16);
	return	256 * x + 16 * y + z;
}

//complete
chunk::chunk* chunk::load(Coord location)
{
	chunk* retchunk = new chunk();
	retchunk->loc =location;
	retchunk->blockstruct = new block[16 * 16 * 16];
	for (int i = 0; i < 16 * 16 * 16; i++)
	{
		retchunk->blockstruct[i] = blockname::block(v3::zeroiv, 0);
		
	
	}
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0;y < 16;y++) {
			for (int z = 0; z < 16; z++)
			{
				int ind = 256 * x + 16 * y + z;
				retchunk->blockstruct[ind].pos = v3::Coord(x , y, z )+location*16;
				int idforblock = minecraftair;
				//select block mechanism
				int ylevel = y + 16 *location.y ;
				
				retchunk->blockstruct[ind].id = minecraftair;
				float noiselevel =  trueperlin(z + 16 * location.z , x  + 16 * location.x)*4+10;
				float val = noiselevel;
				if (noiselevel > ylevel)
				{


					if (val == 13)
					{
						retchunk->blockstruct[ind].id = minecraftgrass;
					}
					if (val < 13)
					{

						retchunk->blockstruct[ind].id = minecraftdirt;

					}
					if (val < 10)
					{

						retchunk->blockstruct[ind].id = minecraftwater;

					}
					if (ylevel <= 6)
					{
						retchunk->blockstruct[ind].id = minecraftdirt;
					}
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
	loc = zeroiv;
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
