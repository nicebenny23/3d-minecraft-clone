#pragma once
#include "item.h"
#include "../block/block.h"
#pragma once 
#define furnaceblockid blockname::minecraftfurnace
inline void initfurnaceitem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->maxamt = 3;
	itm->itemtype = count;
	itm->properties.armor = 1.f;
	itm->properties.placeable = true;
	itm->itemui.itemsprite = createitembox("images\\furnace.png","FurnaceTexture");
}
 // !grassitem_HPP
