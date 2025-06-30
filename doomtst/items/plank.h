#include "item.h"
#include "../block/block.h"
#pragma once 
#define plankitemblockid blockname::minecraftplank
inline void initplankitem(item* itm) {
	itm->properties.pickaxepower = 1;

	itm->maxamt = 64;
	itm->properties.placeable = true;

	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\treestoneblock.png","TreestoneBlockTexture");
}
 // !grassitem_HPP
