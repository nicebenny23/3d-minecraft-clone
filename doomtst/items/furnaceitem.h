#pragma once
#include "item.h"
#include "../block/block.h"
#ifndef furnaceitem_HPP
#define furnaceitem_HPP
#define furnaceblockid blockname::minecraftfurnace
inline void initfurnaceitem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->maxamt = 3;
	itm->itemtype = count;
	itm->properties.armor = .3f;
	itm->properties.placeable = true;
	itm->itemui.itemsprite = createitembox("images\\furnace.png");
}
#endif // !grassitem_HPP
