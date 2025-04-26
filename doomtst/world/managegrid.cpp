#include "managegrid.h"
#include "../block/blockinit.h"


queue<block*> lightingq;
bool gridutil::redoallighting = true;
void gridutil::sendrecreatemsg()
{

	for (int i = 0; i < CtxName::ctx.Grid->totalChunks; i++)
	{
		CtxName::ctx.GridRef()[i]->mesh->meshrecreateneeded = true;
	}

}

void gridutil::createlightingqueue()
{
	lightingq = queue<block*>();
}

void gridutil::computecover(face& blkface)

{

	if (!apx(blkface.mesh->box.scale, blockscale))
	{
		
		blkface.covercomputed = true;
			blkface.covered = false;
			return;
		

	}
	blkface.covered = true;

	
	Coord pos = blkface.mesh->blk->pos + dirfromint(blkface.facenum);
	block* blk = CtxName::ctx.Grid->getBlock(pos);
	if (blk == nullptr)
	{

		
		blkface.covercomputed = false;
		blkface.covered = !blkface.mesh->blk->attributes.transparent;
		return;
	}
	if (blkface.mesh->blk->attributes.transparent)
	{

		blkface.covercomputed = true;
	
		if (blkface.mesh->blk->id!=minecraftair)
		{

			blkface.covered = blk->attributes.transparent && (blk->id == blkface.mesh->blk->id);

		}

	}
	else {
		blkface.covercomputed = true;
		blkface.covered = !blk->attributes.transparent;
		

	}

}
#include <thread>
#include <vector>
#include <mutex>

void processChunks(int startChunk, int endChunk)
{
	
}

void gridutil::computeallcover()
{
		
	for (int gridind = 0; gridind < CtxName::ctx.Grid->totalChunks; ++gridind)
	{
		Chunk::chunk* chk = CtxName::ctx.GridRef()[gridind];
		for (int blockind = 0; blockind < chunksize; ++blockind)
		{
			block& blk = chk->blockbuf[blockind];
			for (int faceind = 0; faceind < 6; ++faceind)
			{
				face& tocover = blk[faceind];
				if (!tocover.covercomputed)
				{
					gridutil::computecover(tocover);
				}
			}
		}
	}
	// Optionally, synchronize final operations like sending messages
	sendrecreatemsg();
}
void gridutil::computepartialcover()
{


}

void gridutil::emitlight()
{
	while (!lightingq.empty())
	{


		block* blk = lightingq.pop();
		if (blk->attributes.transparent)
		{


			for (int i = 0; i < 6; i++)
			{
				block* blocklight = CtxName::ctx.Grid->getBlock(blk->pos + dirfromint(i));
				if (blocklight != nullptr)
				{
					if (blocklight->attributes.transparent) {

						if (blocklight->lightval < blk->lightval - 1)
						{
							blocklight->lightval = blk->lightval - 1;
							lightingq.push(blocklight);
						}
					}
					else
					{
						int blockface = invdirind(i);

						(*blocklight)[blockface].light = blk->lightval;


					}
				}
			}
		}
	}
}


void gridutil::redolighting()
{

	
	if (redoallighting)
	{


		redoallighting = false;

		for (int chunkind = 0; chunkind < CtxName::ctx.Grid->totalChunks; chunkind++)
		{
			for (int blockind = 0; blockind < chunksize; blockind++)
			{
				block* blk =&( (*(CtxName::ctx.GridRef()[chunkind]))[blockind]);
				
				
				for (int faceind = 0; faceind < 6; faceind++)
				{
					
					(blk->mesh.faces)[faceind].light = 0;
				}
			     blk->lightval = blk->emitedlight;
				if (0 < blk->emitedlight)
				{
					lightingq.push(blk);
				}
			}
		}
		emitlight();
	}
	
}
void blockchangecoverupdate(blockname::block* location) {
	
	for (int faceind = 0; faceind < 6; faceind++)
	{
		(location->mesh)[faceind].covercomputed = false;
		gridutil::computecover((location->mesh)[faceind]);
	}
	for (int blkind = 0; blkind < 6; blkind++)
	{
		block* blockatpos = CtxName::ctx.Grid->getBlock(dirfromint(blkind) + location->pos);
		for (int faceind = 0; faceind < 6; faceind++) {
			(blockatpos->mesh)[faceind].covercomputed = false;
			if (blockatpos != nullptr)
			{


				gridutil::computecover(((blockatpos->mesh))[faceind]);
			}
		}
	}
}


void gridutil::gridupdate()
{
	CtxName::ctx.Grid->updateborders();
	CtxName::ctx.Grid->load();
	if (CtxName::ctx.Grid->haschanged())
	{
		gridutil::computeallcover();
		gridutil::redoallighting = true;

	}


	gridutil::redolighting();

}
void blocklightingupdateevent(int prevlight, int newlight, Coord loc) {
	if (newlight >= prevlight)
	{
		CtxName::ctx.Grid->getBlock(loc)->lightval = CtxName::ctx.Grid->getBlock(loc)->emitedlight;
		lightingq.push(CtxName::ctx.Grid->getBlock(loc));
		for (int i = 0; i < 6; i++)
		{
			v3::Vector3 dir = dirfromint(i);
			block* blocklight = CtxName::ctx.Grid->getBlock(loc + dir);
			if (blocklight != nullptr)
			{
				lightingq.push(blocklight);
			}
		}
		gridutil::emitlight();
	}
	else
	{

		gridutil::redoallighting = true;



	}

}
//the setblock function is what should be used
void gridutil::setblock(Coord loc, int blockid)
{
	block* location= CtxName::ctx.Grid->getBlock(loc);

		
		if (location != nullptr)
		{

			int prevemit = CtxName::ctx.Grid->getBlock(loc)->emitedlight;
			CtxName::ctx.Grid->GetChunk(loc)->modified = true;
			blkinitname::setair(location);
			location->id = blockid;
			blkinitname::blockinit(location);
			blockchangecoverupdate(location);
			sendrecreatemsg();
			blocklightingupdateevent(prevemit, location->emitedlight, loc);

		}
}

