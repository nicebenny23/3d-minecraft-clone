#include "playerinventory.h"
#include "../renderer/texture.h"
void inventory::givestartitems(int i1, int i2) {
	if (i1!=0)
	{

		hotbar.at(0).giveitem(i1,10);

	}
	if (i2 != 0)
	{

		hotbar.at(1).giveitem(i2, 100);

	}
}
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
		givestartitems(treestoneitem, crystaldaggeritem);
}
