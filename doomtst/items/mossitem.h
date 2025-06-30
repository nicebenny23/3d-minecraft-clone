#include "item.h"
#include "../block/block.h"
#pragma once 
#define mossitemblockid -1
inline void initmossitem(item* itm) {
	itm->properties.pickaxepower = 1;
	itm->properties.foodval = 2;
	itm->maxamt = 1;
	itm->properties.placeable = true;

	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\mossitem.png","MossItemTexture");
}
 // !grassitem_HPP
