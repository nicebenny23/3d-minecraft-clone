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
					if (newchunk.blockbuf[i].hascomponent<chestcomp>())
					{

						newchunk.blockbuf[i].getcomponent<chestcomp>().men.blkcont.destroy();
						//we created a contaner so we are going back
						currentcontid -= 1;
						int resourceid = randomproperties[i] & 255;

				
						newchunk.blockbuf[i].getcomponent<chestcomp>().men.blkcont=  Container(resourceid);
				
					}
					if (newchunk.blockbuf[i].hascomponent<furnacecomp>())
					{

						newchunk.blockbuf[i].getcomponent<furnacecomp>().men.blkcont.destroy();
						//we created a contaner so we are going back
						currentcontid -= 2;
						int resourceid = randomproperties[i] & 255;

						int newloc = randomproperties[i] / 256.f;
						newchunk.blockbuf[i].getcomponent<furnacecomp>().men.blkcont.resourcecontainer = new Container(resourceid);
						newchunk.blockbuf[i].getcomponent<furnacecomp>().men.blkcont.newitemlocation = new Container(newloc);

					}
					//if (newchunk.blockbuf[i].hascomponent<>())
					
					i++;
				}
			}
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

	if (inrange(biomeval, -.3, .3))
	{
	return normalbiome;
	}
	if (inrange(biomeval,.3,1))
	{
		return mossybiome;
	}
	if (inrange(biomeval, -1, -.3))
	{
		return lavabiome;
	}

}
bool shouldbeore(float feturemap) {

	if (inrange(feturemap, 0, .0002f))
	{
		return true;
	}
	return false;

}

bool shouldbeironore(float feturemap) {

	if (inrange(feturemap, .0002, .001f))
	{
		return true;
	}
	return false;
}
bool shouldbemoss(float feturemap, float distto) {

	if (inrange(feturemap, .0, .1f)||inrange(distto,0,.1f))
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
int idfromnoise(Coord pos, float nint, float mint, float bint, float fint) {

	int neid = minecraftair;
	//if between the value is negitive if above positive
	float distto = 2 * (sigmoid(nint - (mint)) - .5);
	int biomeval = getbiometype(bint);
	bool covered = false;
	if (inrange(nint, -mint, mint))
	{
		covered = true;
		if (biomeval==mossybiome)
		{
			neid = minecrafttreestone;
		}
		else
		{

			neid = minecraftstone;

		}
	}
	if (covered)
	{

		if (shouldbeore(fint))
		{
			neid = minecraftcrystal;
		}
		if (shouldbeironore(fint))
		{
			neid = minecraftironore;
		}

	}
	if (covered)
	{


		switch (biomeval)
		{
		case mossybiome:
			if (shouldbemoss(fint, distto))
			{
				neid = minecraftmoss;
			}
			break;
		case normalbiome:
			break;
		case lavabiome:
			if (shouldbemoss(fint, distto))
			{
				neid = minecraftlava;
			}
			break;
		default:
			break;
		}
	}
	return neid;

}
int generatechunkvalfromnoise(Coord pos,noisemap* map, noisemap* modulatedmap, noisemap* biomemap, noisemap* feturemap,noisemap* lavamap)
{

	if (pos.y>10010)
	{
		return minecraftair;
	}
	if (pos.y>10000)
	{
		return minecraftobsidian;
	}
	Coord localpos;
	localpos.x = modabs(pos.x,16);

	localpos.y = modabs(pos.y, 16);

	localpos.z = modabs(pos.z, 16);
	float nint = (*map)[localpos];
	float bint = (*biomemap)[localpos];
	float fint = (*feturemap)[localpos];
	float mint = (*modulatedmap)[localpos];
	float lint = (*lavamap)[Coord(localpos.x,0,localpos.z)];

	float modval = (mint + 1) / 2;
	modval /= 3;
	bint *= 4.f;
	float biomebias = 2 * (sigmoid(pos.y / 300.f) - .5);
	bint += biomebias;
	bint = clamp(bint, -1.0f, 1.0f);
	nint *= clamp(biomebias + 1, .4, 1.4);
	if (generateflat)
	{
	
		
		if (pos.y<0)
		{
			return minecraftstone;
		}
		return minecraftair;

	}
	if (pos.y==-500)
	{
		return minecraftobsidian;
	}
	if (pos.y<-500)
	{
		int maxy = lint * 30 - 530;
		if (maxy>-520)
		{
			maxy = 1000;
		}
	
		if (pos.y<maxy)
		{
			return minecraftobsidian;

		}
		if (pos.y < -530)
		{
			return minecraftlava;

		}
		return minecraftair;
	}
	return idfromnoise(pos, nint, modval, bint, fint);


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
	noisemap* map = genperlin(location * 16,  2,.5f,.1f, 1.2,normalnoise);
	noisemap* feturemap= genperlin((location + Coord(0, 1010, 0) )* 16 , 1, .5f, .08f, 1.2, normalnoise);

	noisemap* biomemap = genperlin((location+Coord(10,202,0)) * 16, 1, .5f, .003f, 1.2, normalnoise);
	noisemap* lavalayermap= genperlin2d(Vector3(location.x,-10,location.z) *16, 1, .5f, .03f, 1.2, normalnoise);
	noisemap* modulatedmap = genperlin((location +Coord(100,0,0)) * 16, 2, .1f, .1f, 1.2,normalnoise);
	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++)
			{
				Coord blockpos = Coord(x, y, z) + location * 16;


				

			
			
				int neid = generatechunkvalfromnoise(blockpos,map,modulatedmap,biomemap,feturemap,lavalayermap);


				newchunk.blockbuf[ind] = blockname::block(blockpos, neid);

				blkinitname::genblock(&newchunk.blockbuf[ind], neid, blockpos, 0, 0);

				ind++;

			}


		}


	}

	map->destroy();
	modulatedmap->destroy();
	feturemap->destroy();
	biomemap->destroy();

	return &newchunk;
}
