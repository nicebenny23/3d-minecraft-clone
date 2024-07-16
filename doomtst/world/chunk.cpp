#include "chunk.h"
#include "noise.h"
#include "../renderer/chunkrender.h"
#include <string>
#include "../util/fileloader.h"
#include "../block/blockinit.h"
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

Chunk::chunk* Chunk::fileload(Coord location)
{
	
	const char* name = getcorefilename(location);
	safefile file = safefile(name, fileread);
	byte* bytelist = file.read<byte>(4096);
	chunk& newchunk = *(new chunk());
	newchunk.modified = false;
	createchunkmesh(&newchunk);
	newchunk.loc = location;
	newchunk.blockbuf = new block[chunksize];


	int i = 0;
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++)
			{
				Coord blockpos = Coord(x, y, z) + location * 16;
				newchunk.blockbuf[i] = blockname::block(blockpos, bytelist[i]);

				initblockmesh(&newchunk.blockbuf[i], zerov,unitscale);
				blkinitname::blockinit(&newchunk.blockbuf[i]);
				i++;
			}
		}
	}

	delete bytelist;
	file.close();
	return &newchunk;
}


//complete
Chunk::chunk* Chunk::load(Coord location)
{
	if (fileexists(getcorefilename(location)))
	{
		return fileload(location);
	}
	chunk& newchunk = *(new chunk());
	newchunk.modified = false;
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
				
				initblockmesh(&newchunk.blockbuf[ind], zerov,unitscale);
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

				if (abs(noiselevel)<  .11f)
				{
					if (abs(noiselevel )> .1f)
					{

						newchunk.blockbuf[ind].id = minecraftcrystal;
					}
					
				}


				blkinitname::blockinit(&newchunk.blockbuf[ind]);


				ind++;
			}
		}
	}

	map->destroy();

	return &newchunk;
}


const char* Chunk::getcorefilename(Coord pos)
{
	std::string *strng = (new std::string());
	strng->append("worldstorage\\");
	strng->append("t");
	strng->append(std::to_string(pos.x));
	strng->append("t");
	strng->append(std::to_string(pos.y));
	strng->append("t");
	strng->append(std::to_string(pos.z));

return 	strng->data();
}

void Chunk::chunk::write()
{
	
	const char* name = getcorefilename(loc);

	safefile file = safefile(getcorefilename(loc), filewrite);
	array<byte> bytelist = array<byte>();
	for (int i = 0; i < chunksize; i++)
	{
		bytelist[i] = blockbuf[i].id;
	}
	file.write<byte>(bytelist.getdata(), bytelist.length);
	bytelist.destroy();
	file.close();
}

Chunk::chunk::chunk()
{
	loc = zeroiv;
	blockbuf = nullptr;
	mesh = nullptr;
}

void Chunk::chunk::destroy()
{
	if (modified)
	{

		write();
	}
	for (int i = 0; i < 16 * 16 * 16; i++) {
		
		gameobject::immidiatedestroy(&blockbuf[i]);
		delete 	blockbuf[i].mesh;
		//delete blockbuf[i]
	}
	mesh->destroy();
	delete[] blockbuf;
}