#include "../renderer/Window.h"
#include "../world/grid.h"
#include "../world/world.h"
#include "../util/userinput.h"
#include "../game/time.h"
#include "entity.h"
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