#include "loottable.h"
#include "../player/playerinventory.h"
#include "../player/player.h"
void lootelement::drop(ecs::Ecs& world)
{
	int dropamt = maxamt;
	items::item_id id= world.get_resource<items::item_type_register>().unwrap().from_name(item_name);
	items::item_entry entry = items::item_entry(id, maxamt);
	items::stage_transaction_emplaced<items::give_container_entry>(world,entry,player::goblin.get_component<inventory>().hotbar.get_component<items::container>());
}
