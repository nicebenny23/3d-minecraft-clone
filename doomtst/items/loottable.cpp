#include "loottable.h"
#include "../player/playerinventory.h"
#include "../player/player.h"
void lootelement::drop()
{
	int dropamt = maxamt;

	player::goblin.get_component<inventory>().hotbar.fill(itemid, dropamt, false);
	player::goblin.get_component<inventory>().playermenu.blkcont.fill(itemid, dropamt, false);

	player::goblin.get_component<inventory>().hotbar.fill(itemid, dropamt, true);
	player::goblin.get_component<inventory>().playermenu.blkcont.fill(itemid, dropamt, true);
}
