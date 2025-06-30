#include "item.h"
#include "../block/block.h"
#pragma once 
#define chestblockid blockname::minecraftchest
inline void initchestitem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->maxamt = 3;
	itm->itemtype = count;

	itm->properties.placeable = true;
	itm->itemui.itemsprite = createitembox("images\\chest.png","ChestItemTexture");
}
 // !grassitem_HPP
