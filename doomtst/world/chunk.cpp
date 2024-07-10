#include "chunk.h"
#include "noise.h"
#include "../renderer/chunkrender.h"
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

block& Chunk::chunk::operator[](int index)
{
	return blockstruct[index];
}


int Chunk::indexfrompos(int x, int y, int z)
{
	x = modabs1(x, 16);
	y = modabs1(y, 16);
	z = modabs1(z, 16);
	return	256 * x + 16 * y + z;
}

void createchunkmesh(Chunk::chunk* aschunk)
{
	chunkmesh* mesh = new chunkmesh;
	mesh->genbufs();
	aschunk->mesh = mesh;
}


//complete
Chunk::chunk* Chunk::load(Coord location)
{

	chunk& retchunk = *(new chunk());
	
	retchunk.loc =location;
	createchunkmesh(&retchunk);
	retchunk.blockstruct = new block[chunksize];
	int ind = 0;
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0;y < 16;y++) {
			for (int z = 0; z < 16; z++)
			{
				Coord blockpos = Coord(x, y, z) +location * 16;
				retchunk.blockstruct[ind] = blockname::block(blockpos, 0);
				initblockmesh(&retchunk.blockstruct[ind], zerov, unitv / 2.0009);
				//select block mechanism
				
				  
				//todo fix it
				retchunk.blockstruct[ind].id = minecraftair;
				float noiselevel =  trueperlin(blockpos.x,blockpos.z)*5+10;
			
			
				if (noiselevel >= 10)
				{
					if (blockpos.y == floor(noiselevel)) {
						retchunk.blockstruct[ind].id = minecraftgrass;
					}
					if (blockpos.y <floor(noiselevel))
					{
						retchunk.blockstruct[ind].id = minecraftdirt;
					}
					if (blockpos.y < 4)
					{
						retchunk.blockstruct[ind].id = minecraftdirt;
					}
				}
				if (noiselevel < 10)
				{
					
						if (blockpos.y <=10)
						{
							retchunk.blockstruct[ind].id = minecraftwater;
						}
						
					if (blockpos.y <4)
				     {
					retchunk.blockstruct[ind].id = minecraftdirt;
				     }
				}
				
				giveblocktraits(&(retchunk.blockstruct[ind]));
			
				
				ind++;
			}
		}
	}
	

	return & retchunk;
}


Chunk::chunk::chunk()
{
	loc = zeroiv;
	blockstruct =nullptr ;
}

void Chunk::chunk::destroy()
{
	for (int i = 0;i < 16 * 16 * 16;i++) {
		gameobject::objectfromguid[blockstruct[i].guid] = nullptr;
	
	gameobject::immidiatedestroy(&blockstruct[i]);

	}
	mesh->destroy();
	delete[] blockstruct;
}
