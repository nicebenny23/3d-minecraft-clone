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
		window::Window& WinRef() {

			return *Window;
		}
	
		Context() {

			Inp = nullptr;
			Window = nullptr;
			Grid = nullptr;
			Ecs = nullptr;
			wrld = nullptr;
		}
		World::world* wrld;
		userinput::InputManager* Inp;
		window::Window* Window;
	
		ecs::Ecs* Ecs;

		renderer::Renderer* Ren;
		
		grid::Grid* Grid;
		
	};
	extern Context ctx;
	
}