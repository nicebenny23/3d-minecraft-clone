#include "ecs/ecs.h"
#include "../game/close.h"
#include "time.h"
#pragma once
namespace timing{
	struct Ticks :ecs::resource {
		bool tick_frame;
		Ticks(timing::WorldClock& dur) :tick_counter(dur) {

		}
		timing::Duration tick_counter;
	};
	struct TickingSystem :ecs::System {
		void run(ecs::Ecs& world) {
				Ticks& t = world.insert_resource<Ticks>(world.get_resource<timing::WorldClock>()); 
				t.tick_frame = false;
			if (player::in_game(world)) {
				if (t.tick_counter.is_inactive()) {
					t.tick_counter.set(1);
					t.tick_frame = true;
				}

			}
		}
	};
}