#include "loottable.h"
#include "../player/playerinventory.h"
#include "../player/player.h"
void lootelement::drop()
{
	int dropamt = maxamt;

	player::goblin.getcomponent<inventory>().hotbar.fill(itemid, dropamt, false);
	player::goblin.getcomponent<inventory>().playermenu.blkcont.fill(itemid, dropamt, false);

	player::goblin.getcomponent<inventory>().hotbar.fill(itemid, dropamt, true);
	player::goblin.getcomponent<inventory>().playermenu.blkcont.fill(itemid, dropamt, true);
}
