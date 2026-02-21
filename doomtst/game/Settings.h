#include "../game/ecs/ecs.h"
#pragma once
namespace settings {
	struct GlobalSettings:ecs::resource
	{
		
		bool viewmode;
		GlobalSettings() {
			viewmode = false;
		}
	};
}