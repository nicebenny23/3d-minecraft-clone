#include "chunk.h"
#include "noise.h"
#include "../renderer/chunkrender.h"


block& Chunk::chunk::operator[](int index)
{
	return blockbuf[index];
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

	chunk& newchunk = *(new chunk());

	newchunk.loc = location;
	createchunkmesh(&newchunk);
	newchunk.blockbuf = new block[chunksize];
	int ind = 0;
	chunknoisemap* map = trueperlin(location , .1f, 1.2, .5f, 2);
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++)
			{
				Coord blockpos = Coord(x, y, z) + location * 16;
				newchunk.blockbuf[ind] = blockname::block(blockpos, 0);
				gameobject::objectfromguid[newchunk.blockbuf[ind].guid] = &newchunk.blockbuf[ind];

				initblockmesh(&newchunk.blockbuf[ind], zerov, unitv / 2.0009);
				//select block mechanism


				//todo fix it
				newchunk.blockbuf[ind].id = minecraftair;
				float noiselevel =  (*map)[Coord(x, y, z)];
				///	float noiselevel1 = (*map1)[Coord(x, y, z)];

				if (noiselevel > -.15)
				{

					if (noiselevel < .15)
					{

						newchunk.blockbuf[ind].id = minecraftstone;
					}

				}




				giveblocktraits(&(newchunk.blockbuf[ind]));


				ind++;
			}
		}
	}

	map->destroy();

	return &newchunk;
}


Chunk::chunk::chunk()
{
	loc = zeroiv;
	blockbuf = nullptr;
	mesh = nullptr;
}

void Chunk::chunk::destroy()
{
	for (int i = 0; i < 16 * 16 * 16; i++) {
		gameobject::objectfromguid[blockbuf[i].guid] = nullptr;

		gameobject::immidiatedestroy(&blockbuf[i]);
		delete 	blockbuf[i].mesh;
		//delete blockbuf[i]
	}
	mesh->destroy();
	delete[] blockbuf;
}