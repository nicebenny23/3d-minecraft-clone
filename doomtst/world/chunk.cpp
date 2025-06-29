#include "chunk.h"
#include "noise.h"
#include "../renderer/chunkrender.h"
#include <string>
#include "../util/fileloader.h"
#include "../block/blockinit.h"
#include "../block/block.h"
#include "../util/algorthm.h"

#include "../util/dynamicarray.h"
#include "../renderer/renderer.h"
block& Chunk::chunk::operator[](int index)
{
	return blockbuf[index];
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
void appendspecialbytelist(array<short>& bytelist, int index, block* blk) {

	liquidprop* getliq = blk->getcomponentptr<liquidprop>();
	if (getliq != nullptr)
	{
		bytelist[chunksize+ index] = getliq->liqval;

	}
	if (blk->hascomponent<craftingtablecomp>())
	{
		
		bytelist[chunksize+index] =blk->getcomponent<craftingtablecomp>().men.blkcont.getcombinedid();

	}
	if (blk->hascomponent<chestcomp>())
	{
		bytelist[chunksize+ index] = blk->getcomponent<chestcomp>().men.blkcont.containerid;
		
	}
	if (blk->hascomponent<furnacecomp>())
	{
		bytelist[chunksize + index] = blk->getcomponent<furnacecomp>().men.blkcont.getcombinedid();

	}
}
void Chunk::chunk::write()
{

	const char* name = getcorefilename(loc);

	safefile file = safefile(getcorefilename(loc), filewrite);
	array<short> bytelist = array<short>();
	for (int i = 0; i < chunksize; i++)
	{
		int v1= blockbuf[i].id;
		int v2= blockbuf[i].mesh.direction | (blockbuf[i].mesh.attachdir.ind() * 8);
		bytelist[i] = v1 |(256*v2);
	}
	for (int i = 0; i < chunksize; i++)
	{
		bytelist[chunksize + i] = 0;
		appendspecialbytelist(bytelist, i, &blockbuf[i]);
	}
	file.write<short>(bytelist.list, bytelist.length);
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
	{

		write();
	}
	for (int i = 0; i < chunksize; i++) {

		//blkinitname::setair(&blockbuf[i]);
		
	
		gameobject::destroycomponents(&blockbuf[i]);

		//delete blockbuf[i]
	}
	mesh->destroy();
	delete[] blockbuf;
}