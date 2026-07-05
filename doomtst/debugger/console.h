// console.h
#pragma once
#include "../game/ecs/ecs.h"
#include "../imgui/imgui.h"
#include "../game/ecs/resources.h"
namespace console {

	struct Console :ecs::resource {
		void render();
		Console(ecs::Ecs& world) :world(world) {

		}
	private:
		ecs::Ecs& world;
	};

}
