#include "playerinventory.h"
#include "../renderer/texture.h"
inventory::inventory()
{
	texture mtex = texture("slimetex.png", png);
	playermenu = menu(v2::Vector2(.5f, .5f));
		hotbar = Container(6, 1, 0, -4.5);
	hotbar.setviewable(true);
	selected = nullptr;
}
