#pragma once
#include "playermodification.h"
#include "../world/voxeltraversal.h"
#include "../game/rigidbody.h"

#include "../items/loottable.h"
#include "../entities/onhit.h"
#include "../game/particles.h"


void playerbreak::make_drop(gameobject::obj Hit)
{
	if (Hit.hascomponent<loottable>())
	{
		Hit.getcomponent<loottable>().should_drop = true;
	}
}
