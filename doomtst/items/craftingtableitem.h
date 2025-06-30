#include "item.h"
#include "../block/block.h"
#pragma once 
#define craftingtableblockid 9
inline void initcraftingtable(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->maxamt = 3;
	itm->itemtype = count;

	itm->properties.placeable = true;
	itm->itemui.itemsprite = createitembox("images\\craftingtabletop.png","CraftingTableTopTexture");
}
 // !grassitem_HPP
