
#include "chunk.h"
#include "noise.h"
#include "../renderer/chunkrender.h"
#include <string>
#include "../util/interval.h"
#include "../util/fileloader.h"
#include "../block/blockinit.h"
#include <mutex>
#include "../util/thread_split.h"
using namespace blockname;
enum biometype {
	mossybiome=0,
	normalbiome=1,
	open_stone=2,
	
};
biometype get_biome(float biome) {

	if (inter::range(0,1).contains(biome))
	{
		return normalbiome;
	}
	if (inter::range(-1, -.5).contains(biome))
	{
		return normalbiome;
	}
	return mossybiome;

}
int get_default_block(biometype biome) {

	if (biome==normalbiome)
	{
		return minecraftstone;
	}
	if (biome == mossybiome)
	{
		return minecrafttreestone;
	}
	
}
int get_secondary_block(biometype biome) {

	if (biome == normalbiome)
	{
		return minecraftstone;
	}
	if (biome == mossybiome)
	{
		return minecraftmoss;
	}
	return minecrafttreestone;
}
int chaotic_overide(float chaotic, biometype biome) {
	if (inter::range(0.009, .01f).contains(chaotic)&&biome==normalbiome)
	{
		return minecraftcrystal;
	}
	if (inter::range(.01,.03f).contains(chaotic))
	{
		return get_secondary_block(biome);
	}
	if (inter::range(.398, .4f).contains(chaotic)&&biome==normalbiome)
	{
		return minecraftironore;
	}
	return -1;
}
int idfromnoise( float biome,float chaotic,float cave_carve,float cave_carve2,float should_cave ) {
	
	const float offset = .3f;
	biometype biome_type = get_biome(biome);
	int main_block = get_default_block(biome_type);
	
	int overload = chaotic_overide(chaotic,biome_type);
	if (overload!=-1)
	{
		main_block=overload;
	}
	if (.5<chaotic)
	{
		int l = 1;
	}
	if (chaotic<.5 &&0< chaotic)
	{
		int l = 1;
	}
	int neid = main_block;

		if (inrange(cave_carve, -offset, offset))
		{
			if (inrange(cave_carve2, -offset, offset))
			{
				if (0<should_cave)
				{

					neid = minecraftair;
				}
			}
			
			
		}
		if (.99<should_cave)
		{

			neid = minecraftair;
		}
	
	return neid;

}
int generatechunkvalfromnoise(Vec3 pos, noisemap* map, noisemap* crazy, noisemap* slow)
{
	if (generateflat)
	{
		if (0<pos.y)
		{
			return minecraftair;
		}
		return minecraftstone;
	}
	pos *= blocksize;
	
	Coord localpos;
	localpos.x = symmetric_mod (pos.x, chunkaxis);

	localpos.y = symmetric_mod(pos.y, chunkaxis);

	localpos.z = symmetric_mod(pos.z, chunkaxis);
	float carve1 = (*map).Eval(pos + Vec3(0, pos.y, 0));
	float carve2= map->Eval(pos + Coord(0, 103, 40));
	float biome= slow->Eval(pos + Coord(893, 103, 40));
	float cave_region = slow->Eval(pos + Coord(893, 103, 40));
	float random_n = crazy->Eval(pos + Coord(101, 300, 33));

		return idfromnoise(biome, random_n, carve1, carve2, cave_region);
	


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
	noisemap* slow;
	noisemap* map;
	noisemap* middle_map;
	noisemap* crazy;
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
	void iterate(size_t x, std::mutex& mtx)
	{
		int ind = chunkaxis * chunkaxis * x;
			for (int y = 0; y < chunkaxis; y++) {
				for (int z = 0; z < chunkaxis; z++) {
					Coord idpos = loc * chunkaxis + Coord(x, y, z);

					int neid = generatechunkvalfromnoise(idpos, map,crazy,slow);
					mtx.lock();
					ids.reach(ind) = idblock(neid, idpos);
					mtx.unlock();
					ind++;
				}
			}

	}
	idmap(Coord location) {
		slow= genperlin(1,1, .005f, 1.2);
		map = genperlin(3, .6f, .02f, 1.2);
		crazy= genperlin(4, 1.f, .005f, 1.2);
		middle_map = genperlin(2, 1, .005, 1.2);
		array<size_t> x_pos;
		for (auto i = 0; i < chunkaxis; i++)
		{
			x_pos.push(i);
		}
		loc = location;
		ids = array<idblock>();
		std::mutex lck; 
		auto func = [&lck, this](size_t x) {iterate(x, lck); };
		thread_util::par_iter(x_pos.begin(), x_pos.end(), func, 4);
		

	}
	void destroy() {
		map->destroy();
		crazy->destroy();
		slow->destroy();
		middle_map->destroy();
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
		newchunk.blockbuf[i]= Grid->ctx->OC->spawn_empty();
		newchunk.blockbuf[i].addcomponent < block>();

	}
	return &newchunk;
}

Chunk::chunk* ChunkLoader::LoadFromFile(Coord location)
{

	safefile file = safefile(Chunk::getcorefilename(location), fileread);
	unsigned short* bytelist = file.read<unsigned short>(chunksize);

	file.go(chunksize * 2);
	unsigned short* randomproperties = file.read<unsigned short>(chunksize);
	Chunk::chunk& newchunk = *AllocChunk(location);
	int i = 0;
	for (int x = 0; x < chunkaxis; x++)
	{
		for (int y = 0; y < chunkaxis; y++) {
			for (int z = 0; z < chunkaxis; z++)
			{
				Coord blockpos = Coord(x, y, z) + location * chunkaxis;

				byte blockid = bytelist[i] & unsigned char(255);
				byte dirprop = bytelist[i] >> unsigned char(8);


				byte mesh_attachdir = dirprop >> unsigned char(3);
				byte dir = dirprop & unsigned char(7);
				if (dir > 5)
				{
					throw std::logic_error("Directional corruption error");

				}if (mesh_attachdir > 5)
				{
					throw std::logic_error("Directional corruption error");

				}
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
