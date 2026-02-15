#include "../game/ecs/ecs.h"
#include "../world/world.h"
#pragma once


namespace CtxName {

	struct Context
	{
		
		Context() {

			Ecs = nullptr;
			wrld = nullptr;
		}
		World::world* wrld;
		ecs::Ecs* Ecs;

		
	};
	extern Context ctx;
	
}