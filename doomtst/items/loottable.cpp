#include "loottable.h"
#include "../player/playerinventory.h"
#include "../player/player.h"
void loot_element::drop(ecs::Ecs& world) const{
	stn::Option<items::AddToSlotPlan> slot=items::give_container_entry(entry, player::goblin.get_component<inventory>().hotbar.get_component<items::container>());
	if (slot) {
	slot.unwrap().apply(world);
	}
}
