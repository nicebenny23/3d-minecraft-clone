#include "new_slot.h"
#include "../game/GameContext.h"
#include "../game/gameobject.h"
#include "../game/spawn.h"
#pragma once
namespace cont
{
	struct Container {
		stn::array<gameobject::obj> slots;
		size_t fill_amt() {
			for (gameobject::obj )
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
					gameobject::spawn::Spawner spawner(CtxName::ctx.OC);
	gameobject::obj new_slot= spawner.with<slot::ItemSlot>()
						.with<slot::UiSlot>(v2::Coord2(x, y) + lowest).build();	
				slots.push(new_slot);
				}
			}
		}
		void destroy() {
			for (gameobject::obj& elem:slots )
			{
				elem.destroy();
			}
			slots.destroy();
		}

		void Enable() {
			for (gameobject::obj& elem : slots)
			{
				elem.getcomponent<slot::SlotUi>().enable();
			}
		}

		void Disable() {
			for (gameobject::obj& elem : slots)
			{
				elem.getcomponent<slot::SlotUi>().disable();
			}
		}
	};
}