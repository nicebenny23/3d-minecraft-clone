#include "managegrid.h"
queue<block*> lightingq;
void gridutil::sendrecreatemsg()
{

	for (int i = 0; i < totalgridsize; i++)
	{
		chunklist[i]->mesh->meshrecreateneeded = true;
	}

}

void gridutil::createlightingqueue()
{
	lightingq = queue<block*>();
}

void gridutil::computecover(face& blkface)

	{
	if (blkface.mesh->scale== Vector3(1/16.f,3/8.f,1/16.f))
	 {
		blkface.covered = false;
		return;
	}
		Coord pos = blkface.mesh->blk->pos + dirfromint(blkface.facenum);
		block* blk = getobjatgrid(pos, true);
		if (blkface.mesh->blk->transparent)
		{





			if (blk != nullptr)
			{
				if (blk->transparent)
				{



					blkface.covered = ((blk->id == blkface.mesh->blk->id));


				}

			}
			else
			{
				blkface.covered = false;
			}
		}
		else {
			Coord pos = blkface.mesh->blk->pos + dirfromint(blkface.facenum);

			blkface.covered = issolidatpos(pos.x, pos.y, pos.z, true);
		}

	}

void gridutil::computeallcover()
{
	for (int gridind = 0; gridind < totalgridsize; gridind++)

	{

		for (int blockind = 0;blockind < chunksize; blockind++) {

			for (int faceind = 0; faceind < 6; faceind++)
			{
				face& tocover = (*(chunklist[gridind]->blockstruct[blockind]).mesh)[faceind];

				computecover(tocover);
			}

		}
	}
	sendrecreatemsg();
}

void gridutil::emitlight()
{
	while (!lightingq.empty())
	{


		block* blk = lightingq.pop();
		for (int i = 0; i < 6; i++)
		{
			v3::Vector3 dir = dirfromint(i);
			block* blocklight = getobjatgrid(blk->pos + dir, true);
			if (blocklight != nullptr)
			{
				if (blocklight->transparent) {

					if (blocklight->lightval < blk->lightval - 1)
					{
						blocklight->lightval = blk->lightval - 1;
						lightingq.append(blocklight);
					}
				}
				else
				{
					int blockface = i + 2 * (1-modabs(i,2)) - 1;
					
						(*blocklight->mesh)[blockface].light = blk->lightval;
					
					
				}
			}
		}
	}
}


void gridutil::redolighting()
{


	for (int chunkind = 0; chunkind < totalgridsize; chunkind++)
	{
		for (int blockind = 0; blockind < chunksize; blockind++)
		{
			block* blk = &(*chunklist[chunkind])[blockind];
			blk->lightval = 0;
			for (int faceind = 0; faceind < 6; faceind++)
			{
				(*blk->mesh)[faceind].light = 0;
			}
			if (0 < blk->emitedlight)
			{
				blk->lightval = blk->emitedlight;
				lightingq.append(blk);
			}
		}
	}
	emitlight();

}
void gridutil::placeblockatloc(int x, int y, int z, int blockid)

{
		block* location = getobjatgrid(x, y, z);
		if (location != nullptr)
		{

			setair(location);
			location->id = blockid;
			giveblocktraits(location);

			for (int faceind = 0; faceind < 6; faceind++)
			{
				computecover((*location->mesh)[faceind]);
			}
			for (int blkind = 0; blkind < 6; blkind++)
			{
				block* blockatpos = getobjatgrid(dirfromint(blkind) + location->pos);
				for (int faceind = 0;faceind < 6;faceind++) {
					computecover((*(blockatpos->mesh))[faceind]);
					
				}
			}
		}
		sendrecreatemsg();
		redolighting();
}

void gridutil::placeblockatloc(Coord loc, int blockid)
{
	placeblockatloc(loc.x, loc.y, loc.z, blockid);
}

