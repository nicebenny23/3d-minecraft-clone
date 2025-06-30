#include "item.h"
#include "../block/block.h"
#pragma once 
#define sanditemblockid blockname::minecraftsand
inline void sanditeminit(item* itm) {
	itm->properties.pickaxepower = 2;
	itm->properties.placeable = true;
	itm->maxamt = 41;
	itm->amt = 4;
	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\sand.png","SandTexture");
}

 // !rockitemitem_HPP
