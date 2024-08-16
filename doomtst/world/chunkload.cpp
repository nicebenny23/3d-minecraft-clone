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
		short* bytelist = file.read<short>(chunksize);

		file.go(chunksize * 2);
		short* randomproperties = file.read<short>(chunksize);
		Chunk::chunk& newchunk = *(new Chunk::chunk());
		newchunk.modified = false;
		createchunkmesh(&newchunk);
		newchunk.loc = location;
		newchunk.blockbuf = new block[chunksize];


		int i = 0;
		for (int x = 0; x < chunkaxis; x++)
		{
			for (int y = 0; y < chunkaxis; y++) {
				for (int z = 0; z < chunkaxis; z++)
				{
					Coord blockpos = Coord(x, y, z) + location * chunkaxis;
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

	if (inrange(feturemap, .15, .15004f))
	{
		return true;
	}
	return false;

}
bool shouldbesand(float feturemap) {

	if (inrange(feturemap, .3f, .33f))
	{
		return true;
	}
	return false;
}
bool shouldbeironore(float feturemap) {

	if (inrange(feturemap, .20002, .201f))
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
int idfromnoise(Coord pos, float nint, float mint, float bint, float fint,float nint2,float nint3) {
	const float offset = .5f;
	const float offset2 = .85;
	int neid =minecraftstone;

	if (.4 < nint3)
	{


		if (inrange(nint, -offset, offset))
		{
			if (inrange(nint2, -offset, offset))
			{

				neid = minecraftair;
			}
		}
	}
	else
	{
		if (0 < nint3)
		{


			neid = minecraftair;
			if (inrange(bint, -offset2, offset2))
			{
				neid = minecraftstone;
			}
		}
	}
	return neid;

}
int generatechunkvalfromnoise(Vector3 pos,noisemap* map,  noisemap* biomemap, noisemap* feturemap,noisemap* lavamap, noisemap* map2)
{
	pos*= blocksize;
	if (pos.y>10010)
	{
		return minecraftair;
	}
	if (pos.y>10000)
	{
		return minecraftobsidian;
	}
	
	Coord localpos;
	localpos.x = modabs(pos.x, chunkaxis);

	localpos.y = modabs(pos.y, chunkaxis);

	localpos.z = modabs(pos.z, chunkaxis);
	float nint = (*map)[pos + Vector3(0, pos.y , 0)];
	float nint3 = (*map)[Vector3(pos+Vector3(nint*5,0,0))/2 + Coord(0, 103, 40)];
	float nint2 = (*map)[pos+Coord(101,300,33) + Vector3(0, pos.y , 0)];
	float nint4 = 0;
	float fint = 0;
//	float mint = (*modulatedmap)[localpos];
	float lint = 0;
	return idfromnoise(pos, nint, 0, nint2, fint,nint2,nint3);
	const float allow = .9;
	nint4 *= 4.f;
	float biomebias = 2 * (sigmoid(pos.y / 300.f) - .5);
	nint4+= biomebias;
	nint4= clamp(nint4, -1.0f, 1.0f);
	//nint *= clamp(biomebias + 1, .4, 1.4);
	if (generateflat)
	{
	
		
		if (pos.y<0)
		{
			return minecrafttreestone;
		}
		return minecraftair;

	}
	if (pos.y==100)
	{
		return minecraftobsidian;
	}
	if (pos.y<-100)
	{
		int maxy = lint * 30 - 130;
		if (maxy>-120)
		{
			maxy = 1000;
		}
	
		if (pos.y<maxy)
		{
			return minecraftobsidian;

		}
		if (pos.y < -30)
		{
			return minecraftlava;

		}
		return minecraftair;
	}
	return idfromnoise(pos, nint, allow, nint4, fint,0,0);


}
struct idblock {

	int id;
	Coord pos;
	idblock(int blkid, Coord loc) {
		id = blkid;
		pos = loc;
	}
	idblock() {
		id = -1;
		pos = zeroiv;
	}
};

struct idmap
{

	noisemap* map ;
	noisemap* map2;
	noisemap* feturemap ;

	noisemap* biomemap ;
	noisemap* lavalayermap;
	array<idblock> ids;
	Coord loc;
	int idatpos(Coord pos) {

		pos -=  loc* chunkaxis;
		if (inrangeint(int(pos.x),0, chunkaxis-1))
		{
			if (inrangeint(int(pos.y), 0, chunkaxis-1))
			{

				if (inrangeint(int(pos.z), 0, chunkaxis-1))
				{
					return ids[chunkaxis * chunkaxis * pos.x + chunkaxis * pos.y + pos.z].id;
				}
			}
		}
		return generatechunkvalfromnoise(pos+loc* chunkaxis, map, biomemap, feturemap, lavalayermap, map2);

	}
	idmap(Coord location){

		 map = genperlin(1, .6f, .02f, 1.2, rigid);
	 map2 = nullptr;
	 feturemap = nullptr;

		 biomemap = nullptr;
		lavalayermap = genperlin(1, .5f, .03f, 1.2, normalnoise);
		loc = location ;
		ids = array<idblock>(chunksize,false);
		int ind = 0;
		for (int x = 0; x < chunkaxis; x++)
		{
			for (int y = 0; y < chunkaxis; y++) {
				for (int z = 0; z < chunkaxis; z++) {
					Coord idpos = loc * chunkaxis + Coord(x, y, z);
					int neid = generatechunkvalfromnoise(idpos, map, biomemap, feturemap, lavalayermap, map2);
					ids[ind] = idblock(neid, idpos);
						ind++;
				}
			}
			
		}
		
	}
	void destroy() {
		ids.destroy();
		lavalayermap->destroy();
		map->destroy();
		
	}
	void smooth() {
for (int i = 0; i < ids.length; i++)
		{

	int searchid;
	if (ids[i].id == minecraftair) {

		searchid= minecraftstone;
	}
	if (ids[i].id == minecraftstone) {

		searchid = minecraftair;
	}

				int cnt = 0;
				for (int j = 0; j < 6; j++)
				{

					Coord looktopos = ids[i].pos + dirfromint(j);
					if (idatpos(looktopos) == searchid)
					{
						cnt++;

					}
					else
					{
						if (j - cnt >= 3)
						{
							break;
						}
					}
				}
				if (cnt >= 4)
				{
					ids[i].id = searchid;
				}
				continue;

		}

	}
};
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

	idmap mapx = idmap(location);
	mapx.smooth();
	

	int ind = 0;
	for (int x = 0; x < chunkaxis; x++)
	{
		for (int y = 0; y < chunkaxis; y++) {
			for (int z = 0; z < chunkaxis; z++)
			{
				Coord pos = mapx.ids[ind].pos;
				int id = mapx.ids[ind].id;
				newchunk.blockbuf[ind] = blockname::block(pos, id);

				blkinitname::genblock(&newchunk.blockbuf[ind], id,pos, 0, 0);

				ind++;
			}
		}
	}
	mapx.destroy();
	return &newchunk;
}
