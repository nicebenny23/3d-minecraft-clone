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

	if (!apx(blkface.mesh->scale, unitscale))
	{
		
		blkface.covercomputed = true;
			blkface.covered = false;
			return;
		

	}
	blkface.covered = true;

	
	Coord pos = blkface.mesh->blk->pos + dirfromint(blkface.facenum);
	block* blk = getobjatgrid(pos, true);
	if (blk == nullptr)
	{


		blkface.covercomputed = false;
		blkface.covered = !blkface.mesh->blk->attributes.transparent;
		return;
	}
	if (blkface.mesh->blk->attributes.transparent)
	{
		if (blkface.mesh->blk->id!=minecraftair)
		{

			blkface.covercomputed = true;
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
	for (int gridind = startChunk; gridind < endChunk; ++gridind)
	{
		Chunk::chunk* chk = chunklist[gridind];
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
}

void gridutil::computeallcover()
{
	// Determine the number of threads to use
	const int numThreads = 3; // Or choose a specific number

	// Create a vector to hold thread objects
	std::vector<std::thread> threads;

	// Calculate the range of chunks for each thread
	int chunkRange = totalgridsize / numThreads;

	for (int t = 0; t < numThreads; ++t)
	{
		int startChunk = t * chunkRange;
		int endChunk = (t == numThreads - 1) ? totalgridsize : (t + 1) * chunkRange;

		// Create and launch a thread to process the chunk range
		threads.emplace_back(processChunks, startChunk, endChunk);
	}

	// Wait for all threads to complete
	for (auto& th : threads)
	{
		th.join();
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
				Coord dir = dirfromint(i);
				block* blocklight = getobjatgrid(blk->pos + dir, true);
				if (blocklight != nullptr)
				{
					if (blocklight->attributes.transparent) {

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
				
				
				for (int faceind = 0; faceind < 6; faceind++)
				{
					
					(blk->mesh.faces)[faceind].light = 0;
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
void blockchangecoverupdate(blockname::block* location) {

	for (int faceind = 0; faceind < 6; faceind++)
	{
		(location->mesh)[faceind].covercomputed = false;
		gridutil::computecover((location->mesh)[faceind]);
	}
	for (int blkind = 0; blkind < 6; blkind++)
	{
		block* blockatpos = getobjatgrid(dirfromint(blkind) + location->pos);
		for (int faceind = 0; faceind < 6; faceind++) {
			(blockatpos->mesh)[faceind].covercomputed = false;
			if (blockatpos != nullptr)
			{


				gridutil::computecover(((blockatpos->mesh))[faceind]);
			}
		}
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

		blockchangecoverupdate(location);
	}
}

void gridutil::destroyblockatloc(int x, int y, int z)
{
	block* location = getobjatgrid(x, y, z);

	if (location != nullptr)
	{
	//	location->senddestroycall();
		blkinitname::setair(location);
		blockchangecoverupdate(location);

	}

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
void blockchangelighting(int prevlight, int newlight, Coord loc) {
	if (newlight >= prevlight)
	{
		grid::getobjatgrid(loc, true)->lightval = grid::getobjatgrid(loc, true)->emitedlight;
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

	sendrecreatemsg();

	int currlightemit = grid::getobjatgrid(loc, true)->emitedlight;
	blockchangelighting(prevemit, currlightemit, loc);
}

