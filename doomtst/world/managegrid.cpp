#pragma once
#include "managegrid.h"
#include "../block/blockinit.h"
#include "../game/multi_query.h"


queue<block*> lightingq;
bool gridutil::redoallighting = true;
void gridutil::sendrecreatemsg()
{

	for (int i = 0; i < CtxName::ctx.Grid->totalChunks; i++)
	{
		if (CtxName::ctx.GridRef()[i]!=nullptr)
		{

			CtxName::ctx.GridRef()[i]->mesh->meshrecreateneeded = true;

		}	
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
		
			blkface.cover= cover_state::Uncovered;
			return;
		

	}

	
	Coord pos = blkface.mesh->blk->pos+ blkface.facenum.ToVec();
	block* blk = CtxName::ctx.Grid->getBlock(pos);
	if (blk == nullptr)
	{

		
		blkface.cover= blockname::cover_state::Uncomputed;
		//blkface.covered = !blkface.mesh->blk->attributes.transparent;
		return;
	}
	if (blkface.mesh->blk->attributes.transparent)
	{

	
		if (blkface.mesh->blk->id!=minecraftair)
		{
			if (blk->attributes.transparent && (blk->id == blkface.mesh->blk->id))
			{


				blkface.cover = cover_state::Covered;

			}
			else {
				blkface.cover = cover_state::Uncovered;

			}
		}

	}
	else {
	
		if (blk->attributes.transparent)
		{

			blkface.cover = cover_state::Uncovered;

		}
		else {
			blkface.cover = cover_state::Covered;

		}

	}

}
#include <thread>
#include <vector>
#include <mutex>

void processChunks(int startChunk, int endChunk)
{
	
}
void Iter(std::tuple<blockname::block*> block) {
	auto [blk] = block;
	for (int faceind = 0; faceind < 6; ++faceind)
	{
		face& tocover = (*blk)[faceind];
		if (tocover.cover == cover_state::Uncomputed)
		{
			gridutil::computecover(tocover);
		}
	}

}

void gridutil::computeallcover()
{
	query::View<block> blk(CtxName::ctx.OC);
	multi_query<block>(blk, std::function(Iter), 4);
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


			for (auto dir: Dir::Directions3d)
			{
				block* blocklight = CtxName::ctx.Grid->getBlock(blk->pos +dir.ToVec());
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
						int blockface = dir.Inv().ind();

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
			if (CtxName::ctx.GridRef()[chunkind]==nullptr)
			{
				continue;
			}
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
		(location->mesh)[faceind].cover = cover_state::Uncomputed;
		gridutil::computecover((location->mesh)[faceind]);
	}
	for (int blkind = 0; blkind < 6; blkind++)
	{
		block* blockatpos = CtxName::ctx.Grid->getBlock(Dir::Dir3d(blkind).ToVec() + location->pos);
		for (int faceind = 0; faceind < 6; faceind++) {
			
			if (blockatpos != nullptr)
			{
				(blockatpos->mesh)[faceind].cover = cover_state::Uncomputed;

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
			v3::Coord dir = Dir::Dir3d(i).ToVec();
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

