#include "playermodification.h"
#include "../world/voxeltraversal.h"
void initbreakparticle(entityname::entity* newent)
{
	newent->transform.position = newent->transform.position;

		newent->transform.position += Vector3(random()-.5, 1, random()-.5)/5;

		
		newent->addcomponent<aabb::Collider>(newent->transform.position, unitscale / 22, true,true);
		newent->addcomponentptr<rigidbody>(1, .1)->velocity = Vector3(random()-.5, random()-.2, random()-.5) * 2;
		newent->transform.scale = unitscale / 22;
		newent->addcomponent<destroyonhit>("!particle");

	
}
