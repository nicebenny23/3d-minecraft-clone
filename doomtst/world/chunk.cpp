
#include "chunk.h"
#include "noise.h"
#include "../renderer/chunkrender.h"
#include <string>
#include "../util/fileloader.h"
#include "../block/blockinit.h"
#include "../block/block.h"
#include <format>
#include "../util/algorthm.h"#include "../util/dynamicarray.h"
#include "../renderer/renderer.h"
gameobject::obj& Chunk::chunk::operator[](size_t index)
{
	return blockbuf[index];
}


size_t Chunk::indexfrompos(Coord pos)
{
	int x = symmetric_mod(pos.x, chunkaxis);
	int y = symmetric_mod(pos.y, chunkaxis);
	int z = symmetric_mod(pos.z, chunkaxis);
	return	chunkaxis* chunkaxis * x + chunkaxis * y + z;
}


void Chunk::chunkmesh::genbufs()
{

	
	SolidGeo= CtxName::ctx.Ren->gen_renderable();
	SolidGeo.set_material("SolidBlock");
	SolidGeo.set_layout(vertice::vertex().push<float, 3>().push<float, 3>().push<float, 1>());
	TransparentGeo = CtxName::ctx.Ren->gen_renderable();
	TransparentGeo.set_material("TransparentBlock");
	TransparentGeo.set_layout(vertice::vertex().push<float, 3>().push<float, 3>().push<float, 1>());
	
}



int compare(const void* b, const void* a) {
	return sign(((face*)a)->cameradist - ((face*)b)->cameradist);
}
void Chunk::chunkmesh::sortbuf()
{
	for (int i = 0; i < facebuf.length(); i++)
	{
		facebuf[i].calccameradist();
	}
	std::qsort(facebuf.data(), facebuf.length(), sizeof(face), compare);

}


void Chunk::chunkmesh::destroy()
{			 
	SolidGeo.destroy();
	TransparentGeo.destroy();
	
	facebuf.clear();
}

void Chunk::createchunkmesh(Chunk::chunk* aschunk)
{
	chunkmesh* mesh = new chunkmesh;
	mesh->genbufs();
	aschunk->mesh = mesh;

}
Chunk::chunk* Chunk::airload(Coord location)
{
	chunk& newchunk = *(new chunk());
	return &newchunk;
}
//complete



std::string  Chunk::getcorefilename(Coord pos)
{

	std::string m = std::format("Chunk{}", pos);
	std::filesystem::path path = CtxName::ctx.wrld->get_path();
	path = path / "Chunks" /m;  // assign the combined path back
	return 	path.string();
}
//this whole system has to be completly redone
void appendspecialbytelist(array<unsigned short>& bytelist, int index, block* blk) {

	liquidprop* getliq = blk->owner.getcomponentptr<liquidprop>();
	if (getliq != nullptr)
	{
		bytelist[chunksize+ index] = getliq->liqval;

	}
	if (blk->owner.hascomponent<craftingtablecomp>())
	{
		
		bytelist[chunksize+index] =blk->owner.getcomponent<craftingtablecomp>().men.blkcont.getcombinedid();

	}
	if (blk->owner.hascomponent<chestcomp>())
	{
		bytelist[chunksize+ index] = blk->owner.getcomponent<chestcomp>().men.blkcont.containerid;
		
	}
	if (blk->owner.hascomponent<furnacecomp>())
	{
		bytelist[chunksize + index] = blk->owner.getcomponent<furnacecomp>().men.blkcont.getcombinedid();

	}
}
void Chunk::chunk::write()
{


	safefile file = safefile(getcorefilename(loc), filewrite);
	array<unsigned short> bytelist = array<unsigned short >();
	for (int i = 0; i < chunksize; i++)
	{
		size_t v1= blockbuf[i].getcomponent<block>().id;
		size_t dir = blockbuf[i].getcomponent<block>().mesh.direction.ind();
		size_t attach = (blockbuf[i].getcomponent<block>().mesh.attachdir.ind());
		
		size_t v2 = dir | attach << 3;
		size_t fin = v1 | (v2 << 8);
		if (dir>5||attach>5)
		{
			throw std::logic_error("Directional corruption error");
		}
		bytelist.push(fin);
	}
	for (int i = 0; i < chunksize; i++)
	{
		bytelist.reach(chunksize + i) = 0;
		appendspecialbytelist(bytelist, i, blockbuf[i].getcomponentptr<block>());
	}
	file.write<unsigned short>(bytelist.data(), bytelist.length());
	
	file.close();
}

Chunk::chunk::chunk()
{
	loc = ZeroCoord;
	modified = false;
	blockbuf = nullptr;
	mesh = nullptr;
}

void Chunk::chunk::destroy()
{
	if (modified)
	{		write();
	}
	for (int i = 0; i < chunksize; i++) {	
	blockbuf[i].destroy();
	}
	mesh->destroy();
	delete[] blockbuf;
}