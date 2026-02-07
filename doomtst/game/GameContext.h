#include "../world/grid.h"
#include "../world/world.h"
#pragma once


namespace CtxName {

	struct Context
	{
		
		grid::Grid& GridRef(){

			return *Grid;
		}
		
		Context() {

			Grid = nullptr;
			Ecs = nullptr;
			wrld = nullptr;
		}
		World::world* wrld;
		ecs::Ecs* Ecs;

		grid::Grid* Grid;
		
	};
	extern Context ctx;
	
}