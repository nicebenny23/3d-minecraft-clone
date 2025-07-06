#include "../renderer/Window.h"
#include "../world/grid.h"
#include "../util/userinput.h"
#include "../util/time.h"
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
		timename::TimeManager& TimeRef() {

			return *Time;
		}
		Context() {

			Inp = nullptr;
			Window = nullptr;
			Grid = nullptr;
			Time = nullptr;
			OC = nullptr;
			
		}
		userinput::InputManager* Inp;
		window::Window* Window;
		timename::TimeManager* Time;

		gameobject::OCManager* OC;

		renderer::Renderer* Ren;
		
		grid::Grid* Grid;
		
	};
	extern Context ctx;
	
}