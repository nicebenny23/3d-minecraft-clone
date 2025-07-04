
#include "chunk.h"
#include "noise.h"
#include "../renderer/chunkrender.h"
#include <string>
#include "../util/fileloader.h"
#include "../block/blockinit.h"
#include "../block/block.h"
#include "../util/algorthm.h"#include "../util/dynamicarray.h"
#include "../renderer/renderer.h"
block& Chunk::chunk::operator[](int index)
{
	return blockbuf[index].getcomponent<block>();
}


int Chunk::indexfrompos(Coord pos)
{
	int x = symmetric_mod(pos.x, chunkaxis);
	int y = symmetric_mod(pos.y, chunkaxis);
	int z = symmetric_mod(pos.z, chunkaxis);
	return	chunkaxis* chunkaxis * x + chunkaxis * y + z;
}


void Chunk::chunkmesh::genbufs()
{
	renderer::Ren.Gen<true>(&TransparentGeo);

	renderer::Ren.Gen<true>(&SolidGeo);
	SolidGeo.AddAttribute<float,3>().AddAttribute<float,3>().AddAttribute<float, 1>();
	TransparentGeo.AddAttribute<float, 3>().AddAttribute<float, 3>().AddAttribute<float, 1>();

}



int compare(const void* b, const void* a) {
	return sign(((face*)a)->cameradist - ((face*)b)->cameradist);
}
void Chunk::chunkmesh::sortbuf()
{
	for (int i = 0; i < facebuf.length; i++)
	{
		facebuf[i].calccameradist();
	}
	std::qsort(facebuf.list, facebuf.length, sizeof(face), compare);

}


void Chunk::chunkmesh::destroy()
{			 

	renderer::Ren.Destroy(&SolidGeo);
	renderer::Ren.Destroy(&TransparentGeo);

	
	facebuf.destroy();
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



const char* Chunk::getcorefilename(Coord pos)
{
	std::string* strng = new std::string();
	strng->append("worldstorage\\Chunk");
	strng->append(std::to_string(pos.x));
	strng->append(",");
	strng->append(std::to_string(pos.y));
	strng->append(",");
	strng->append(std::to_string(pos.z));
	return 	strng->data();
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

	const char* name = getcorefilename(loc);

	safefile file = safefile(getcorefilename(loc), filewrite);
	array<unsigned short> bytelist = array<unsigned short >();
	for (int i = 0; i < chunksize; i++)
	{
		size_t v1= blockbuf[i].getcomponent<block>().id;
		size_t dir = blockbuf[i].getcomponent<block>().mesh.direction;
		size_t attach = (blockbuf[i].getcomponent<block>().mesh.attachdir.ind());
		
		size_t v2 = dir | attach << 3;
		size_t fin = v1 | (v2 << 8);
		if (dir>5||attach>5)
		{
			throw std::logic_error("Directional corruption error");
		}
		bytelist[i] = fin;
	}
	for (int i = 0; i < chunksize; i++)
	{
		bytelist[chunksize + i] = 0;
		appendspecialbytelist(bytelist, i, blockbuf[i].getcomponentptr<block>());
	}
	file.write<unsigned short>(bytelist.list, bytelist.length);
	bytelist.destroy();
	file.close();
}

Chunk::chunk::chunk()
{
	loc = zeroiv;
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
	blockbuf[i].immediate_destroy();
	}
	mesh->destroy();
	delete[] blockbuf;
}