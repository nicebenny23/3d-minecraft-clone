#include "item.h"
#pragma once 
#define slimewanditem_iid 9
inline void initslimeitem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->properties.placeable = false;
	itm->maxamt = 100;
	itm->properties.dmg = 4;

	itm->itemtype = wear;
	itm->itemui.itemsprite = createitembox("images\\slime_wand.png", "slime_wand_tex");
}
// !sworditem_HPP
