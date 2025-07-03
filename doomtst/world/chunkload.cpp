
#include "chunk.h"
#include "noise.h"
#include "../renderer/chunkrender.h"
#include <string>
#include "../util/fileloader.h"
#include "../block/blockinit.h"


using namespace blockname;

enum biometype {
	mossybiome=0,
	normalbiome=1,
	lavabiome=2,

};
int idfromnoise( float nint, float bint,  float nint3) {
	const float offset = .5f;
	const float offset2 = .85;
	int neid = minecraftstone;

	if (.4 < nint3)
	{


		if (inrange(nint, -offset, offset))
		{
			if (inrange(bint, -offset, offset))
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
int generatechunkvalfromnoise(Vector3 pos, noisemap* map, noisemap* biomemap, noisemap* feturemap, noisemap* lavamap, noisemap* map2)
{
	pos *= blocksize;
	
	Coord localpos;
	localpos.x = symmetric_mod (pos.x, chunkaxis);

	localpos.y = symmetric_mod(pos.y, chunkaxis);

	localpos.z = symmetric_mod(pos.z, chunkaxis);
	float nint = (*map).Eval(pos + Vector3(0, pos.y, 0));
	float nint3 = map->Eval(pos + Coord(0, 103, 40));
	float nint2 = map->Eval(pos + Coord(101, 300, 33));

		return idfromnoise( nint, nint2,nint3);
	


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

	noisemap* map;
	noisemap* map2;
	noisemap* feturemap;

	noisemap* biomemap;
	noisemap* lavalayermap;
	array<idblock> ids;
	Coord loc;
	int idatpos(Coord pos) {

		pos -= loc * chunkaxis;
		if (inrange(int(pos.x), 0, chunkaxis - 1))
		{
			if (inrange(int(pos.y), 0, chunkaxis - 1))
			{

				if (inrange(int(pos.z), 0, chunkaxis - 1))
				{
					return ids[chunkaxis * chunkaxis * pos.x + chunkaxis * pos.y + pos.z].id;
				}
			}
		}
		return minecraftair;

	}
	idmap(Coord location) {

		map = genperlin(1, .6f, .02f, 1.2, rigid);
		map2 = nullptr;
		feturemap = nullptr;

		biomemap = nullptr;
		lavalayermap = genperlin(1, .5f, .03f, 1.2, normalnoise);
		loc = location;
		ids = array<idblock>(chunksize);
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

};
	Chunk::chunk* ChunkLoader::AllocChunk(Coord location)
	{
		Chunk::chunk& newchunk = *(new Chunk::chunk());
		newchunk.modified = false;
		createchunkmesh(&newchunk);
		newchunk.loc = location;
		newchunk.blockbuf = new gameobject::obj[chunksize];
		for (int i = 0; i < chunksize; i++) {
			Grid->ctx->OC->InitObj(&newchunk.blockbuf[i]);
			newchunk.blockbuf[i].addcomponent < block>();
		
		}
		return &newchunk;
	}

	Chunk::chunk* ChunkLoader::LoadFromFile(Coord location)
	{

		const char* name = Chunk::getcorefilename(location);
		safefile file = safefile(name, fileread);
		short* bytelist = file.read<short>(chunksize);

		file.go(chunksize * 2);
		short* randomproperties = file.read<short>(chunksize);
		Chunk::chunk& newchunk = *AllocChunk(location);
		int i = 0;
		for (int x = 0; x < chunkaxis; x++)
		{
			for (int y = 0; y < chunkaxis; y++) {
				for (int z = 0; z < chunkaxis; z++)
				{
					Coord blockpos = Coord(x, y, z) + location * chunkaxis;

					byte blockid = bytelist[i] & 255;
					byte dirprop = bytelist[i] >> 8;


					byte mesh_attachdir = dirprop >> 3;
					byte dir = dirprop & 7;
					blkinitname::genblock(newchunk.blockbuf[i].getcomponentptr<block>(), blockid, blockpos, mesh_attachdir, dir);

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


						newchunk.blockbuf[i].getcomponent<chestcomp>().men.blkcont = Container(resourceid);

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


	Chunk::chunk* ChunkLoader::LoadFromNoise(Coord location)
	{

		Chunk::chunk& newchunk = *AllocChunk(location);
		idmap statemap = idmap(location);
		

		int ind = 0;
		for (int x = 0; x < chunkaxis; x++)
		{
			for (int y = 0; y < chunkaxis; y++) {
				for (int z = 0; z < chunkaxis; z++)
				{
					Coord pos = statemap.ids[ind].pos;
					int id = statemap.ids[ind].id;


					blkinitname::genblock(newchunk.blockbuf[ind].getcomponentptr<block>(), id, pos, 0, 0);

					ind++;
				}
			}
		}
		statemap.destroy();
		return &newchunk;
	}
	void ChunkLoader::Init(CtxName::Context* ctx)
	{
		Grid = ctx->Grid;
	}
	Chunk::chunk* ChunkLoader::LoadChunk(Coord location)
	{
		if (fileexists(Chunk::getcorefilename(location)))
		{
			return LoadFromFile(location);
		}
		return LoadFromNoise(location);
	}
