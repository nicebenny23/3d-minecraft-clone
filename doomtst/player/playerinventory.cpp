#include "playerinventory.h"
#include "../renderer/texture.h"
inventory::inventory()
{
	texture mtex = texture("slimetex.png", png);
	playermenu = menu(v2::Vector2(.5f, .5f));
	playerheld = new item(1);
	playerheld->state = beingheld;
}
