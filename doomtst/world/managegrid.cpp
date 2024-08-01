#include "managegrid.h"
#include "../block/blockinit.h"
queue<block*> lightingq;
bool gridutil::redoallighting = true;
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

	if (!apx(blkface.mesh->scale, (unitscale)))
	{
		

			blkface.covered = false;
			return;
		

	}
	blkface.covered = true;


	Coord pos = blkface.mesh->blk->pos + dirfromint(blkface.facenum);
	block* blk = getobjatgrid(pos, true);
	if (blk == nullptr)
	{
		blkface.covered = !blkface.mesh->blk->transparent;
		return;
	}
	if (blkface.mesh->blk->transparent)
	{
		blkface.covered = blk->transparent && (blk->id == blkface.mesh->blk->id);


	}
	else {
		blkface.covered = !blk->transparent;
		

	}

}

void gridutil::computeallcover()
{
	for (int gridind = 0; gridind < totalgridsize; gridind++)

	{
		
		for (int blockind = 0; blockind < chunksize; blockind++) {

			for (int faceind = 0; faceind < 6; faceind++)
			{

				face* tocover = &(chunklist[gridind]->blockbuf[blockind])[faceind];

				computecover(*tocover);
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
		if (blk->transparent)
		{


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
						int blockface = invdir(i);

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

		for (int chunkind = 0; chunkind < totalgridsize; chunkind++)
		{
			for (int blockind = 0; blockind < chunksize; blockind++)
			{
				block* blk = &(*chunklist[chunkind])[blockind];
				blk->lightval = 0;
				for (int faceind = 0; faceind < 6; faceind++)
				{
					(blk->mesh)[faceind].light = 0;
				}
				blk->lightval = blk->emitedlight;
				if (0 < blk->emitedlight)
				{
					lightingq.append(blk);
				}
			}
		}
		emitlight();
	}
}
void gridutil::placeblockatloc(int x, int y, int z, int blockid)

{
	block* location = getobjatgrid(x, y, z);
	int prevlight = location->emitedlight;
	if (location != nullptr)
	{

		blkinitname::setair(location);
		location->id = blockid;
		blkinitname::blockinit(location);

		for (int faceind = 0; faceind < 6; faceind++)
		{
			computecover((location->mesh)[faceind]);
		}
		for (int blkind = 0; blkind < 6; blkind++)
		{
			block* blockatpos = getobjatgrid(dirfromint(blkind) + location->pos);
			for (int faceind = 0; faceind < 6; faceind++) {

				if (blockatpos != nullptr)
				{


					computecover(((blockatpos->mesh))[faceind]);
				}
			}
		}
	}
	sendrecreatemsg();
	redoallighting = true;


}

void gridutil::destroyblockatloc(int x, int y, int z)
{
	block* location = getobjatgrid(x, y, z);

	if (location != nullptr)
	{
		location->senddestroycall();
		blkinitname::setair(location);
		
		for (int faceind = 0; faceind < 6; faceind++)
		{
			computecover((location->mesh)[faceind]);
		}
		for (int blkind = 0; blkind < 6; blkind++)
		{
			block* blockatpos = getobjatgrid(dirfromint(blkind) + location->pos);
			for (int faceind = 0; faceind < 6; faceind++) {
				if (blockatpos!=nullptr)
				{

					computecover((blockatpos->mesh)[faceind]);

				}
			}
		}
	}
	sendrecreatemsg();
	redoallighting = true;

}

void gridutil::gridupdate()
{
	grid::reupdatechunkborders();
	grid::load();
	if (grid::gridchanged())
	{
		gridutil::computeallcover();
		gridutil::redoallighting = true;

	}


	gridutil::redolighting();

}

//the setblock function is what should be used
void gridutil::setblock(Coord loc, int blockid)
{
	block* curblock = getobjatgrid(loc, true);

	int prevemit = grid::getobjatgrid(loc, true)->emitedlight;
	chunkatpos(loc.x, loc.y, loc.z)->modified = true;
	if (blockid != minecraftair)
	{
		placeblockatloc(loc.x, loc.y, loc.z, blockid);
	}
	else
	{
		destroyblockatloc(loc.x, loc.y, loc.z);

	}
	int currlightemit = grid::getobjatgrid(loc, true)->emitedlight;
	bool transparent = grid::getobjatgrid(loc, true)->transparent;
	if (currlightemit >= prevemit)
	{
		gridutil::redoallighting = true;

		lightingq.append(grid::getobjatgrid(loc, true));
		for (int i = 0; i < 6; i++)
		{
			v3::Vector3 dir = dirfromint(i);
			block* blocklight = getobjatgrid(loc + dir, true);
			if (blocklight != nullptr)
			{
				lightingq.append(blocklight);
			}
		}
		emitlight();
	}
	else
	{

		//redolighting();
		gridutil::redoallighting = true;

		

	}
}

