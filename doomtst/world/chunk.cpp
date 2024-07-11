#include "chunk.h"
#include "noise.h"
#include "../renderer/chunkrender.h"


block& Chunk::chunk::operator[](int index)
{
	return blockstruct[index];
}


int Chunk::indexfrompos(int x, int y, int z)
{
	x = modabs(x, 16);
	y = modabs(y, 16);
	z = modabs(z, 16);
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
	chunknoisemap* map = trueperlin(location);
	chunknoisemap* map2 = trueperlin(location+Coord(1,333,2));
	chunknoisemap* map3 = trueperlin(location + Coord(3331,22,3));
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0;y < 16;y++) {
			for (int z = 0; z < 16; z++)
			{
				Coord blockpos = Coord(x, y, z) +location * 16;
				retchunk.blockstruct[ind] =blockname::block(blockpos, 0);
				gameobject::objectfromguid[retchunk.blockstruct[ind].guid] = &retchunk.blockstruct[ind];
				
				initblockmesh(&retchunk.blockstruct[ind], zerov, unitv / 2.0009);
				//select block mechanism
			
				  
				//todo fix it
				retchunk.blockstruct[ind].id = minecraftair;
				float noiselevel = (*map)[ind];
				float noiselevel2 = (*map)[ind];
				float noiselevel3 = (*map)[ind];
				noiselevel = interpolate(noiselevel, noiselevel2, (noiselevel3 + 1) / 2);
				
				if (noiselevel >=-.5)
				{
					if (noiselevel <= .5)
					{
						if (noiselevel2 >= -.6)
						{
							if (noiselevel2 <= .6)
							{
								retchunk.blockstruct[ind].id = minecraftstone;
							}
						}
					}
					
				}
				

				
				
				giveblocktraits(&(retchunk.blockstruct[ind]));
			
				
				ind++;
			}
		}
	}
	
	map->destroy();
	map2->destroy();
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
	delete 	blockstruct[i].mesh;
	//delete blockstruct[i]
	}
	mesh->destroy();
	delete[] blockstruct;
}
