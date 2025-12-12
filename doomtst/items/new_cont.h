#include "new_slot.h"
#include "../game/GameContext.h"
#include "../game/ecs/game_object.h"
#include "../game/spawn.h"
#pragma once
namespace cont
{
	struct Container {
		stn::array<ecs::obj> slots;
		size_t fill_amt() {
			for (ecs::obj )
			{

			}

		}
		void give(item_name::item& itm, size_t cnt) {
			

		}
		Container(v2::Coord2 grid, v2::Coord2 lowest) {
			for (size_t y = 0; y++; y < grid.y)
			{
				for (size_t x = 0; x++; x < grid.x)
				{
					ecs::spawn::Spawner spawner(CtxName::ctx.OC);
	ecs::obj new_slot= spawner.with<slot::ItemSlot>()
						.with<slot::UiSlot>(v2::Coord2(x, y) + lowest).build();	
				slots.push(new_slot);
				}
			}
		}
		void destroy() {
			for (ecs::obj& elem:slots )
			{
				elem.destroy();
			}
			slots.destroy();
		}

		void Enable() {
			for (ecs::obj& elem : slots)
			{
				elem.get_component<slot::SlotUi>().enable();
			}
		}

		void Disable() {
			for (ecs::obj& elem : slots)
			{
				elem.get_component<slot::SlotUi>().disable();
			}
		}
	};
}