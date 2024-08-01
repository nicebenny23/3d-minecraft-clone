#include "../player/player.h"
#include "../util/random.h"
#include "slime.h"
#include "../world/grid.h"
#include"../world/voxeltraversal.h"
#include "../renderer/blockrender.h"
#ifndef entityspawner_HPP
#define entityspawner_HPP
#define enemytag 1
void spawn() {
	if (!blockrender::enablelighting)
	{
		return;
	}
	float spawnthreshold=.001f;
	float randomnum = random();
	
	if (randomnum<spawnthreshold)
	{
		
	Vector3 pos= voxtra::findemptyspace(unitv);
				if (distance(player::goblin.toent()->transform.position,pos)>8)
				{
					int entityswitchnum = randomint(4);
					createslime(pos);
				}
			
				
	
	}
	



}

#endif // !entityspawner_HPP
