#include "chunk.h"
#include "noise.h"
#include "../renderer/chunkrender.h"
#include <string>
#include "../util/fileloader.h"
#include "../block/blockinit.h"



using namespace blockname;
Chunk::chunk* chunkfileload(Coord location)
{
	
		const char* name = Chunk::getcorefilename(location);
		safefile file = safefile(name, fileread);
		short* bytelist = file.read<short>(4096);

		file.go(4096 * 2);
		short* randomproperties = file.read<short>(4096);
		Chunk::chunk& newchunk = *(new Chunk::chunk());
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

						int newloc = randomproperties[i] / 256.f;
						newchunk.blockbuf[i].getcomponent<craftingtablecomp>().men.blkcont.resourcecontainer = new Container(resourceid);
						newchunk.blockbuf[i].getcomponent<craftingtablecomp>().men.blkcont.newitemlocation = new Container(newloc);

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
enum biometype {
	mossybiome=0,
	normalbiome=1,
	lavabiome=2,

};
biometype getbiometype(float biomeval){

	if (inrange(biomeval, -.1, .1))
	{
	return normalbiome;
	}
	if (inrange(biomeval,.1,.7))
	{
		return mossybiome;
	}
	if (inrange(biomeval, -.7, -.1))
	{
		return lavabiome;
	}

}
bool shouldbeore(float feturemap) {

	if (inrange(feturemap, 0, .02f))
	{
		return true;
	}
	return false;
}
bool shouldbemoss(float feturemap, float distto) {

	if (inrange(feturemap, .0, .1)||distto<.02)
	{
		return true;
	}

	return false;
}

bool shouldbelava(float feturemap) {

	if (inrange(feturemap, .06, .1))
	{
		return true;
	}

	return false;
}

int generatechunkvalfromnoise(float noiselevel,float modulatedmap,float biomemap,float feturemap)
{


	float modval = (modulatedmap + 1) / 2;
	modval /= 3;
	int neid = minecraftair;
	if (generateflat)
	{
	

		return neid;
	}
	float distto =sigmoid(noiselevel-abs(modval));
	if (inrange(noiselevel, -modval, modval))
	{

		neid = minecraftstone;
		
	}
	if (shouldbeore)
	{
		neid = minecraftcrystal;
	}
	int biomeval = getbiometype(biomemap);
	switch (biomeval)
	{
	case mossybiome:
		if (shouldbemoss(feturemap,distto))
		{
			neid = minecraftmoss;
		}
		break;
	case normalbiome:
		break;
	case lavabiome:
	
		break;
	default:
		break;
	}

	return neid;



}

Chunk::chunk* chunkload(Coord location)
{

	if (fileexists(Chunk::getcorefilename(location)))
	{
		return chunkfileload(location);
	}
	Chunk::chunk& newchunk = *(new Chunk::chunk());
	newchunk.modified = false;
	newchunk.loc = location;
	createchunkmesh(&newchunk);
	newchunk.blockbuf = new block[chunksize];
	int ind = 0;
	chunknoisemap* map = genperlin(location,  2,.5f,.1f, 1.2,normalnoise);
	chunknoisemap* feturemap= genperlin(location+Coord(0,1010,0), 2, .5f, .01f, 1.2, normalnoise);

	chunknoisemap* biomemap = genperlin(location+Coord(10,202,0), 1, .5f, .01f, 1.2, normalnoise);
	chunknoisemap* map2 = genperlin(location+Coord(100,0,0), 2, .1f, .1f, 1.2,normalnoise);
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++)
			{
				Coord blockpos = Coord(x, y, z) + location * 16;


				

				float nint = (*map)[Coord(x, y, z)];
				float bint = (*biomemap)[Coord(x, y, z)];
				float fint = (*feturemap)[Coord(x, y, z)];
				float mint = (*map2)[Coord(x, y, z)];
				
			
				int neid = generatechunkvalfromnoise(nint,mint,bint,fint);


				newchunk.blockbuf[ind] = blockname::block(blockpos, neid);

				blkinitname::genblock(&newchunk.blockbuf[ind], neid, blockpos, 0, 0);

				ind++;

			}


		}


	}

	map->destroy();
	map2->destroy();


	return &newchunk;
}
