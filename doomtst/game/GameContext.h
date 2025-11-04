#include "../renderer/Window.h"
#include "../world/grid.h"
#include "../world/world.h"
#include "../util/userinput.h"
#include "../util/time.h"
#include "entity.h"
#include "System.h"
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
		timename::TimeManager& TimeRef() {

			return *Time;
		}
		Context() {

			Inp = nullptr;
			Window = nullptr;
			Grid = nullptr;
			Time = nullptr;
			OC = nullptr;
			wrld = nullptr;
		}
		World::world* wrld;
		userinput::InputManager* Inp;
		window::Window* Window;
		timename::TimeManager* Time;
		Systems* Executor;
		gameobject::Ecs* OC;

		renderer::Renderer* Ren;
		
		grid::Grid* Grid;
		
	};
	extern Context ctx;
	
}