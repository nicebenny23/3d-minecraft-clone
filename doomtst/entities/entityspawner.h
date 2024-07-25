#include "../player/player.h"
#include "../util/random.h"
#include "slime.h"
#include "../world/grid.h"
#include"../world/voxeltraversal.h"
#ifndef entityspawner_HPP
#define entityspawner_HPP

void spawn() {
	float spawnthreshold=.001;
	float randomnum = random();
	
	if (randomnum<spawnthreshold)
	{
		bool notinblock = false;
		do
		{

			Box spawnradbox;
			float ranx = (random() - .5) * 2;

			float rany = (random() - .5) * 2;

			float ranz = (random() - .5) * 2;
			v3::Vector3 offset = (Vector3(ranx, rany, ranz) * (2 * loadamt + 1) / 2 + gridpos) * 16;
			Box loadbox = Box(offset, unitv * 2);
			if (!voxtra::Boxcollwithgrid(loadbox,false))
			{
				notinblock = true;
				createslime(offset);
			}
		} while (!notinblock);
	}
	



}

#endif // !entityspawner_HPP
