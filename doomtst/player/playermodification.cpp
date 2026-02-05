#pragma once
#include "playermodification.h"
#include "../world/voxeltraversal.h"
#include "../game/rigidbody.h"

#include "../items/loottable.h"
#include "../entities/onhit.h"
#include "../game/particles.h"


void playerbreak::make_drop(ecs::obj Hit)
{
	if (Hit.has_component<items::loot_table>())
	{
		Hit.get_component<items::loot_table>().should_drop = true;
	}
}
