#include "playerinventory.h"
#include "../renderer/texture.h"

inventory::inventory()

{

	selectedind = -1;
	texture mtex = texture("images\\slimetex.png", png);
	playermenu = inventorymen(v2::Vector2(.5f, .5f));
	playermenu.menutype = inventorymenu;
	inventorylocation = &playermenu;
		hotbar = Container(6, 1, 0, -4.5);
		hotbar.enable();
		selected = nullptr;
}
