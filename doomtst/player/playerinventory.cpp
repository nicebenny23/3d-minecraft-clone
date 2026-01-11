#pragma once
#include "playerinventory.h"
void inventory::givestartitems(int i1, int i2,int i3) {
	if (i1!=0)
	{

		hotbar.at(0).giveitem(i1,10);

	}
	if (i2 != 0)
	{

		hotbar.at(1).giveitem(i2, 100);

	}
	if (i3 != 0)
	{

		hotbar.at(2).giveitem(i3, 100);

	}
}
inventory::inventory(): playermenu(v2::Vec2(.5f, .5f)){


}
