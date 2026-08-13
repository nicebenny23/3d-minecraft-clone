#include "ecs/ecs.h"
#include "../game/close.h"
#include "time.h"
#pragma once
namespace timing{
	struct GameClock :ecs::resource {
		bool tick_frame;
		Clock game_clock;
		GameClock() :tick_counter(game_clock) {
			tick_frame = false;
		}
		timing::Duration tick_counter;
	
	};
	struct TickingSystem :ecs::System {
		void run(ecs::Ecs& world) {
			GameClock& t = world.insert_resource<GameClock>(); 
			t.tick_frame = false;
			t.game_clock.add(0);
			if (player::in_game(world)) {
				if (t.tick_counter.is_inactive()) {
					t.tick_counter.set(1);
					t.tick_frame = true;
				}
				t.game_clock.add(world.get_resource<timing::GlobalClock>().dt());
			}
		}
	};
}