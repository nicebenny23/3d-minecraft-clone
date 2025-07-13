#pragma once
#include "playermodification.h"
#include "../world/voxeltraversal.h"
#include "../game/rigidbody.h"
#include "../entities/onhit.h"
#include "../game/particles.h"
void initbreakparticle(gameobject::obj newent)
{
	newent.transform().position = newent.transform().position;

		newent.transform().position += Vec3(random() - .5, 1, random() - .5) / 5;

		
		newent.addcomponent<aabb::Collider>(newent.transform().position, blockscale / 22, true, true);
		newent.addcomponent<rigidbody>(1, .1)->velocity = Vec3(random()-.5, random()-.2, random()-.5) * 2;
		newent.transform().scale = blockscale / 22;
		newent.addcomponent<destroyonhit<particle>>();

	
}
