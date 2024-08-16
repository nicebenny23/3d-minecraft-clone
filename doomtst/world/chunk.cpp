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


int Chunk::indexfrompos(int x, int y, int z)
{
	x = modabs(x, chunkaxis);
	y = modabs(y, chunkaxis);
	z = modabs(z, chunkaxis);
	return	chunkaxis* chunkaxis * x + chunkaxis * y + z;
}


void Chunk::chunkmesh::genbufs()
{
	Voa.generate();
	VBO.generate(GL_ARRAY_BUFFER);
	ibo.generate(GL_ELEMENT_ARRAY_BUFFER);

	transparentVoa.generate();
	transparentVBO.generate(GL_ARRAY_BUFFER);
	transparentibo.generate(GL_ELEMENT_ARRAY_BUFFER);
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
	transparentVoa.destroy();

	transparentVBO.destroy();
	transparentibo.destroy();

	Voa.destroy();
	VBO.destroy();
	ibo.destroy();

	
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
	int newind = 0;
	newchunk.loc = location;
	int ind = 0;
	createchunkmesh(&newchunk);
	newchunk.blockbuf = new block[chunksize];
	for (int x = 0; x < chunkaxis; x++)
	{
		for (int y = 0; y < chunkaxis; y++) {
			for (int z = 0; z < chunkaxis; z++)
			{
				Coord blockpos = Coord(x, y, z) + location * chunkaxis;
				int neid = minecraftair;
				newchunk.blockbuf[ind] = blockname::block(blockpos, neid);
				if (neid==minecraftair)
				{
					newind++;
				}
				blkinitname::genblock(&newchunk.blockbuf[ind], neid, blockpos, 0, 0);

				ind++;
			}
		}
	}
	std::cout << newind;
	return &newchunk;
}
//complete



const char* Chunk::getcorefilename(Coord pos)
{
	std::string* strng = (new std::string());
	strng->append("worldstorage\\Chunk");
	strng->append(std::to_string(pos.x));
	strng->append(",");
	strng->append(std::to_string(pos.y));
	strng->append(",");
	strng->append(std::to_string(pos.z));
	return 	strng->data();
}
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
		int v2= blockbuf[i].mesh.direction | (blockbuf[i].mesh.attachdir * 8);
		bytelist[i] = v1 |(256*v2);
	}
	for (int i = 0; i < chunksize; i++)
	{
		bytelist[chunksize + i] = 0;
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