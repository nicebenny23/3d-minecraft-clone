#include "chunk.h"
#include "noise.h"
#include "../renderer/chunkrender.h"
#include <string>
#include "../util/fileloader.h"
#include "../block/blockinit.h"
#include "../block/block.h"
#include "../renderer/algorthm.h"

#include "../util/dynamicarray.h"
#include "../renderer/renderer.h"
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


void Chunk::chunkmesh::genbufs()
{
	Voa.generate();
	VBO.generate(GL_ARRAY_BUFFER);
	ibo.generate(GL_ELEMENT_ARRAY_BUFFER);
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
	std::qsort(facebuf.getdata(), facebuf.length, sizeof(face), compare);

}


void Chunk::chunkmesh::destroy()
{
	Voa.destroy();
	ibo.destroy();
	VBO.destroy();
	datbuf.destroy();
	indbuf.destroy();
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
void appendspecialbytelist(array<short>& bytelist, int index, block* blk) {

	liquidprop* getliq = blk->getcomponentptr<liquidprop>();
	if (getliq != nullptr)
	{
		bytelist[4096+ index] = getliq->liqval;

	}
	if (blk->hascomponent<craftingtablecomp>())
	{
		
		bytelist[4096+index] =blk->getcomponent<craftingtablecomp>().men.blkcont.getcombinedid();

	}
	if (blk->hascomponent<chestcomp>())
	{
		bytelist[4096 + index] = blk->getcomponent<chestcomp>().men.blkcont.containerid;
		
	}
	if (blk->hascomponent<furnacecomp>())
	{
		bytelist[4096 + index] = blk->getcomponent<furnacecomp>().men.blkcont.getcombinedid();

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
		int v2= blockbuf[i].mesh.direction | (blockbuf[i].mesh.attachdir * 8);
		bytelist[i] = v1 |(256*v2);
	}
	for (int i = 0; i < chunksize; i++)
	{
		bytelist[4096 + i] = 0;
		appendspecialbytelist(bytelist, i, &blockbuf[i]);
	}
	file.write<short>(bytelist.getdata(), bytelist.length);
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