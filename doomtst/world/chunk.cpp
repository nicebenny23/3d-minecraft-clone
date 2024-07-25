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
	short* bytelist = file.read<short>(4096);

	file.go(4096 * 2);
	short* randomproperties = file.read<short>(4096);
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
				byte blockid = bytelist[i] & 255;
				newchunk.blockbuf[i] = blockname::block(blockpos, blockid);

				initblockmesh(&newchunk.blockbuf[i], zerov, unitscale);
				byte dirprop = bytelist[i] >> 8;


				byte attachdir = dirprop >> 3;
				byte dir = dirprop & 7;
				newchunk.blockbuf[i].mesh.attachdir = attachdir;


				newchunk.blockbuf[i].mesh.direction = dir;
				blkinitname::blockinit(&newchunk.blockbuf[i]);
				
				if (newchunk.blockbuf[i].hascomponent<liquidprop>())
				{
					newchunk.blockbuf[i].getcomponent<liquidprop>().liqval = randomproperties[i];
				}
				if (newchunk.blockbuf[i].hascomponent<craftingtablecomp>())
				{

					newchunk.blockbuf[i].getcomponent<craftingtablecomp>().men.blkcont.destroy();
					//we created a contaner so we are going back
					currentcontid -= 2;
					int resourceid = randomproperties[i] & 255;

					int newloc= randomproperties[i]/256.f;
					newchunk.blockbuf[i].getcomponent<craftingtablecomp>().men.blkcont.resourcecontainer =new Container(resourceid);
					newchunk.blockbuf[i].getcomponent<craftingtablecomp>().men.blkcont.newitemlocation= new Container(newloc);

				}

				i++;
			}
		}
	}

	for (int ind = 0; ind < 4096; ind++)
	{


	}
	delete[] bytelist;
	file.close();
	return &newchunk;
}

int generatechunkvalfromnoise(float noiselevel, Coord position,float feturemap,float modulatedmap) {

	

	int neid = minecraftair;
	if (generateflat)
	{
		if (position.y <0)
		{
			neid = minecraftdirt;

		}


	}
	else {

		///	float noiselevel1 = (*map1)[Coord(x, y, z)];

		if (inrange( noiselevel ,-.18f,.18f)&&inrange(modulatedmap,-.18f,.18f))
		{
			//select block mechanism
			neid = minecraftstone;

			if (inrange(feturemap, .1f, .101f))
			{
				neid = minecraftcrystal;

			}
			if (inrange(feturemap, .0f, .02f))
			{
				neid = minecraftdirt;

			}

			}
	
		
	}
	return neid;

}
Chunk::chunk* Chunk::airload(Coord location)
{
	chunk& newchunk = *(new chunk());
	newchunk.modified = false;
	newchunk.loc = location;
	int ind = 0;
	createchunkmesh(&newchunk);
	newchunk.blockbuf = new block[chunksize];
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++)
			{
				Coord blockpos = Coord(x, y, z) + location * 16;
				int neid = minecraftair;
				newchunk.blockbuf[ind] = blockname::block(blockpos, neid);

				blkinitname::genblock(&newchunk.blockbuf[ind], neid, blockpos, 0, 0);

				ind++;
			}
		}
	}
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
	chunknoisemap* map = trueperlin(location, .1f, 1.2, .5f, 2);

	chunknoisemap* expmap =  trueperlin(location+Coord(101,303,2), .1f, 1.2, .5f, 2);
	chunknoisemap* map2 = trueperlin(location+Coord(3,3,33), .04f, 1.2, .5f, 1);
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++)
			{
				Coord blockpos = Coord(x, y, z) + location * 16;


				float expmapval = (*expmap)[Coord(x, y, z)];
				float interp = (1+(*map2)[Coord(x, y, z)])/2;

				float bint = (*map)[Coord(x, y, z)];
				float noiseval = interpolate(expmapval, bint, interp);
				float modulated = (noiseval - bint);
				int neid = generatechunkvalfromnoise(noiseval, blockpos,noiseval,modulated);
				
		
				newchunk.blockbuf[ind] = blockname::block(blockpos, neid);

				blkinitname::genblock(&newchunk.blockbuf[ind], neid, blockpos, 0, 0);

				ind++;

			}


		}
			
		
	}

	map->destroy();
	map2->destroy();
	expmap->destroy();
	return &newchunk;
}


const char* Chunk::getcorefilename(Coord pos)
{
	std::string* strng = (new std::string());
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
		bytelist[2 * i] = blockbuf[i].id;
		bytelist[2 * i + 1] = blockbuf[i].mesh.direction | (blockbuf[i].mesh.attachdir * 8);
	}
	for (int i = 0; i < chunksize; i++)
	{
		bytelist[8192 + 2 * i] = 0;
		bytelist[8192 + 2 * i + 1] = 0;
		liquidprop* getliq = blockbuf[i].getcomponentptr<liquidprop>();
		if (getliq!=nullptr)
		{
			bytelist[8192 + 2 * i] =getliq->liqval;
			bytelist[8192 + 2 * i + 1] = 0;
		}
		if (blockbuf[i].hascomponent<craftingtablecomp>())
		{
			bytelist[8192 + 2 * i] = blockbuf[i].getcomponent<craftingtablecomp>().men.blkcont.resourcecontainer->containerid;
			bytelist[8192 + 2 * i+1] = blockbuf[i].getcomponent<craftingtablecomp>().men.blkcont.newitemlocation->containerid;

			}
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
	for (int i = 0; i < chunksize; i++) {

		//blkinitname::setair(&blockbuf[i]);
		gameobject::immidiatedestroy(&blockbuf[i],false);

		//delete blockbuf[i]
	}
	mesh->destroy();
	delete[] blockbuf;
}